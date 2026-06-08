(() => {
  "use strict";

  const $ = (selector) => document.querySelector(selector);

  const pretty = (value) => JSON.stringify(value, null, 2);

  const eventsState = {
    limit: 20,
    offset: 0,
    loading: false,
    done: false,
    total: null,
  };

  const setText = (selector, value) => {
    const element = $(selector);

    if (!element) {
      return;
    }

    element.textContent = value;
  };

  const setOutput = (selector, value) => {
    const element = $(selector);

    if (!element) {
      return;
    }

    element.textContent = typeof value === "string" ? value : pretty(value);
  };

  const fetchJson = async (url, options = {}) => {
    const response = await fetch(url, {
      headers: {
        Accept: "application/json",
        ...(options.headers || {}),
      },
      ...options,
    });

    const text = await response.text();

    let data = null;

    try {
      data = text ? JSON.parse(text) : null;
    } catch {
      data = {
        ok: false,
        error: {
          code: "invalid_json_response",
          message: text,
        },
      };
    }

    if (!response.ok) {
      const message =
        data?.error?.message ||
        data?.message ||
        `Request failed with status ${response.status}`;

      throw new Error(message);
    }

    return data;
  };

  const postJson = async (url, payload) => {
    return fetchJson(url, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(payload),
    });
  };

  const escapeHtml = (value) => {
    return String(value)
      .replaceAll("&", "&amp;")
      .replaceAll("<", "&lt;")
      .replaceAll(">", "&gt;")
      .replaceAll('"', "&quot;")
      .replaceAll("'", "&#039;");
  };

  const formatPayload = (payload) => {
    if (payload === null || payload === undefined || payload === "") {
      return "—";
    }

    if (typeof payload === "string") {
      return payload;
    }

    try {
      return JSON.stringify(payload);
    } catch {
      return String(payload);
    }
  };

  const normalizeEvents = (payload) => {
    if (Array.isArray(payload)) {
      return payload;
    }

    if (Array.isArray(payload?.events)) {
      return payload.events;
    }

    if (Array.isArray(payload?.data)) {
      return payload.data;
    }

    return [];
  };

  const getEventsTotal = (payload) => {
    return payload?.total ?? payload?.count ?? payload?.events_count ?? null;
  };

  const updateEventsFooter = () => {
    const button = $("#load-more-events");
    const count = $("#events-count");

    if (count) {
      if (eventsState.total !== null) {
        count.textContent = `Showing ${eventsState.offset} of ${eventsState.total} runtime events`;
      } else {
        count.textContent = `Showing ${eventsState.offset} runtime event${
          eventsState.offset > 1 ? "s" : ""
        }`;
      }
    }

    if (button) {
      button.disabled = eventsState.loading || eventsState.done;
      button.textContent = eventsState.loading
        ? "Loading…"
        : eventsState.done
          ? "No more events"
          : "Load more events";
    }
  };

  const renderEventRows = (events, { append = false } = {}) => {
    const body = $("#events-table-body");

    if (!body) {
      return;
    }

    if (!append) {
      body.innerHTML = "";
    }

    if (!Array.isArray(events) || events.length === 0) {
      if (!append) {
        body.innerHTML = `
          <tr>
            <td colspan="5">No events yet.</td>
          </tr>
        `;
      }

      return;
    }

    const html = events
      .map((event) => {
        return `
          <tr>
            <td>${escapeHtml(event.id ?? "—")}</td>
            <td>${escapeHtml(event.source ?? "—")}</td>
            <td>${escapeHtml(event.type ?? "—")}</td>
            <td>${escapeHtml(formatPayload(event.payload))}</td>
            <td>${escapeHtml(event.created_at ?? event.created ?? "—")}</td>
          </tr>
        `;
      })
      .join("");

    body.insertAdjacentHTML("beforeend", html);
  };

  const loadEvents = async ({ reset = false } = {}) => {
    const body = $("#events-table-body");

    if (!body || eventsState.loading) {
      return;
    }

    if (reset) {
      eventsState.offset = 0;
      eventsState.done = false;
      eventsState.total = null;

      body.innerHTML = `
        <tr>
          <td colspan="5">Loading events…</td>
        </tr>
      `;
    }

    if (eventsState.done) {
      updateEventsFooter();
      return;
    }

    eventsState.loading = true;
    updateEventsFooter();

    try {
      const payload = await fetchJson(
        `/api/events?limit=${eventsState.limit}&offset=${eventsState.offset}`,
      );

      const events = normalizeEvents(payload);
      const total = getEventsTotal(payload);

      if (total !== null) {
        eventsState.total = Number(total);
      }

      renderEventRows(events, {
        append: !reset,
      });

      eventsState.offset += events.length;

      if (events.length < eventsState.limit) {
        eventsState.done = true;
      }

      if (
        eventsState.total !== null &&
        eventsState.offset >= eventsState.total
      ) {
        eventsState.done = true;
      }
    } catch (error) {
      body.innerHTML = `
        <tr>
          <td colspan="5">Failed to load events: ${escapeHtml(error.message)}</td>
        </tr>
      `;

      eventsState.done = true;
    } finally {
      eventsState.loading = false;
      updateEventsFooter();
    }
  };

  const loadStatus = async () => {
    try {
      const [health, status, threadpool] = await Promise.all([
        fetchJson("/api/health"),
        fetchJson("/api/status"),
        fetchJson("/api/threadpool"),
      ]);

      setText("#metric-health", health?.status || "ok");
      setText(
        "#metric-health-note",
        health?.ok ? "Runtime is reachable" : "Health check failed",
      );

      setText("#metric-uptime", String(status?.uptime_seconds ?? "—"));
      setText("#metric-boot-count", String(status?.boot_count ?? "—"));
      setText("#metric-events", String(status?.events?.total ?? "—"));
      setText("#metric-http", String(status?.http_requests ?? "—"));
      setText("#metric-ws", String(status?.ws_messages ?? "—"));

      setText("#detail-app", status?.app || "—");
      setText("#detail-runtime", status?.runtime || "—");
      setText("#detail-status", status?.status || "—");
      setText("#detail-heartbeat", status?.last_heartbeat || "—");

      const tp = threadpool?.metrics ? threadpool : status?.threadpool;
      const metrics = tp?.metrics || {};
      const stats = tp?.stats || {};

      setText("#threadpool-running", String(tp?.running ?? "—"));
      setText("#threadpool-workers", String(metrics?.workers ?? "—"));
      setText("#threadpool-pending", String(metrics?.pending ?? "—"));
      setText(
        "#threadpool-completed",
        String(stats?.completed ?? metrics?.completed ?? "—"),
      );
      setText(
        "#threadpool-failed",
        String(stats?.failed ?? metrics?.failed ?? "—"),
      );
    } catch (error) {
      setText("#metric-health", "Error");
      setText("#metric-health-note", error.message);

      const body = $("#events-table-body");

      if (body) {
        body.innerHTML = `
          <tr>
            <td colspan="5">Status load failed: ${escapeHtml(error.message)}</td>
          </tr>
        `;
      }
    }
  };

  const setupRefreshButton = () => {
    const button = $("#refresh-status");

    if (!button) {
      return;
    }

    button.addEventListener("click", async () => {
      button.disabled = true;
      button.textContent = "Refreshing…";

      await loadStatus();
      await loadEvents({ reset: true });

      button.disabled = false;
      button.textContent = "Refresh";
    });
  };

  const setupEventsPagination = () => {
    const button = $("#load-more-events");

    if (!button) {
      return;
    }

    button.addEventListener("click", async () => {
      await loadEvents();
    });
  };

  const setupKvForm = () => {
    const form = $("#kv-form");

    if (!form) {
      return;
    }

    form.addEventListener("submit", async (event) => {
      event.preventDefault();

      const key = $("#kv-key")?.value?.trim() || "";
      const value = $("#kv-value")?.value || "";

      if (!key) {
        setOutput("#kv-output", "Key is required.");
        return;
      }

      try {
        setOutput("#kv-output", "Writing KV value…");

        const written = await postJson("/api/kv", {
          key,
          value,
        });

        const read = await postJson("/api/kv/read", {
          key,
        });

        setOutput("#kv-output", {
          written,
          read,
        });

        await loadStatus();
        await loadEvents({ reset: true });
      } catch (error) {
        setOutput("#kv-output", {
          ok: false,
          error: error.message,
        });
      }
    });
  };

  const setupJobForm = () => {
    const form = $("#job-form");

    if (!form) {
      return;
    }

    form.addEventListener("submit", async (event) => {
      event.preventDefault();

      const name = $("#job-name")?.value?.trim() || "status-page-job";
      const value = Number($("#job-value")?.value || 0);

      try {
        setOutput("#job-output", "Running job…");

        const result = await postJson("/api/jobs", {
          name,
          value,
        });

        setOutput("#job-output", result);

        await loadStatus();
        await loadEvents({ reset: true });
      } catch (error) {
        setOutput("#job-output", {
          ok: false,
          error: error.message,
        });
      }
    });
  };

  const setupWebSocketInfoButton = () => {
    const button = $("#ws-info-button");

    if (!button) {
      return;
    }

    button.addEventListener("click", async () => {
      try {
        setOutput("#ws-output", "Loading WebSocket info…");

        const [info, metrics] = await Promise.all([
          fetchJson("/api/ws/info"),
          fetchJson("/api/ws/metrics"),
        ]);

        setOutput("#ws-output", {
          info,
          metrics,
        });

        await loadStatus();
        await loadEvents({ reset: true });
      } catch (error) {
        setOutput("#ws-output", {
          ok: false,
          error: error.message,
        });
      }
    });
  };

  const setupWebSocketTestButton = () => {
    const button = $("#ws-connect-button");

    if (!button) {
      return;
    }

    button.addEventListener("click", () => {
      const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
      const host = window.location.hostname || "localhost";
      const url = `${protocol}//${host}:9090/`;

      setOutput("#ws-output", `Connecting to ${url}…`);

      let socket;

      try {
        socket = new WebSocket(url);
      } catch (error) {
        setOutput("#ws-output", {
          ok: false,
          error: error.message,
        });
        return;
      }

      const messages = [];

      const push = (entry) => {
        messages.push(entry);
        setOutput("#ws-output", messages.join("\n"));
      };

      socket.addEventListener("open", () => {
        push("connected");

        socket.send(
          JSON.stringify({
            type: "ping",
            payload: {
              source: "pico-status-page",
            },
          }),
        );
      });

      socket.addEventListener("message", async (event) => {
        push(`received: ${event.data}`);

        try {
          await loadStatus();
          await loadEvents({ reset: true });
        } catch {
          // The WebSocket test should not fail only because status refresh failed.
        }

        socket.close();
      });

      socket.addEventListener("close", () => {
        push("closed");
      });

      socket.addEventListener("error", () => {
        push("error: WebSocket connection failed");
        push(
          "Check that websocket.enabled=true and websocket.port=9090 in .env",
        );
      });
    });
  };

  const init = async () => {
    setupRefreshButton();
    setupEventsPagination();
    setupKvForm();
    setupJobForm();
    setupWebSocketInfoButton();
    setupWebSocketTestButton();

    if ($("#metric-health")) {
      await loadStatus();
    }

    if ($("#events-table-body")) {
      await loadEvents({ reset: true });
    }
  };

  document.addEventListener("DOMContentLoaded", () => {
    init().catch((error) => {
      console.error(error);
    });
  });
})();
