(() => {
  "use strict";

  const $ = (selector) => document.querySelector(selector);

  const pretty = (value) => JSON.stringify(value, null, 2);

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

  const loadStatus = async () => {
    try {
      const [health, status, threadpool, events] = await Promise.all([
        fetchJson("/api/health"),
        fetchJson("/api/status"),
        fetchJson("/api/threadpool"),
        fetchJson("/api/events"),
      ]);

      setText("#metric-health", health?.status || "ok");
      setText(
        "#metric-health-note",
        health?.ok ? "Runtime is reachable" : "Health check failed",
      );

      setText("#metric-uptime", String(status?.uptime_seconds ?? "—"));
      setText("#metric-boot-count", String(status?.boot_count ?? "—"));
      setText(
        "#metric-events",
        String(status?.events?.total ?? events?.count ?? "—"),
      );
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

      renderEvents(events?.events || []);
    } catch (error) {
      setText("#metric-health", "Error");
      setText("#metric-health-note", error.message);
      setOutput("#events-table-body", error.message);
    }
  };

  const renderEvents = (events) => {
    const body = $("#events-table-body");

    if (!body) {
      return;
    }

    if (!Array.isArray(events) || events.length === 0) {
      body.innerHTML = `
        <tr>
          <td colspan="5">No events yet.</td>
        </tr>
      `;
      return;
    }

    body.innerHTML = events
      .map((event) => {
        return `
          <tr>
            <td>${escapeHtml(event.id ?? "")}</td>
            <td>${escapeHtml(event.source ?? "")}</td>
            <td>${escapeHtml(event.type ?? "")}</td>
            <td>${escapeHtml(event.payload ?? "")}</td>
            <td>${escapeHtml(event.created_at ?? "")}</td>
          </tr>
        `;
      })
      .join("");
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

      button.disabled = false;
      button.textContent = "Refresh";
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
    setupKvForm();
    setupJobForm();
    setupWebSocketInfoButton();
    setupWebSocketTestButton();

    if ($("#metric-health") || $("#events-table-body")) {
      await loadStatus();
    }
  };

  document.addEventListener("DOMContentLoaded", () => {
    init().catch((error) => {
      console.error(error);
    });
  });
})();
