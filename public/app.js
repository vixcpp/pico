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

  const authState = {
    sessionId: "",
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

  const deleteJson = async (url, payload) => {
    return fetchJson(url, {
      method: "DELETE",
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

  const setupAuthForm = () => {
    const form = $("#auth-form");

    if (!form) {
      return;
    }

    const readCredentials = () => {
      return {
        email: $("#auth-email")?.value?.trim() || "",
        password: $("#auth-password")?.value || "",
      };
    };

    const requireCredentials = () => {
      const credentials = readCredentials();

      if (!credentials.email) {
        setOutput("#auth-output", "Email is required.");
        return null;
      }

      if (!credentials.password) {
        setOutput("#auth-output", "Password is required.");
        return null;
      }

      return credentials;
    };

    const registerButton = $("#auth-register-button");
    const loginButton = $("#auth-login-button");
    const checkButton = $("#auth-check-button");
    const logoutButton = $("#auth-logout-button");
    const deleteButton = $("#auth-delete-button");

    if (registerButton) {
      registerButton.addEventListener("click", async () => {
        const credentials = requireCredentials();

        if (!credentials) {
          return;
        }

        try {
          setOutput("#auth-output", "Registering user…");

          const result = await postJson("/api/auth/register", credentials);

          setOutput("#auth-output", result);

          await loadStatus();
          await loadEvents({ reset: true });
        } catch (error) {
          setOutput("#auth-output", {
            ok: false,
            error: error.message,
          });
        }
      });
    }

    if (loginButton) {
      loginButton.addEventListener("click", async () => {
        const credentials = requireCredentials();

        if (!credentials) {
          return;
        }

        try {
          setOutput("#auth-output", "Logging in…");

          const result = await postJson("/api/auth/login", credentials);

          authState.sessionId = result?.session?.id || "";

          setOutput("#auth-output", {
            ...result,
            saved_session_id: authState.sessionId ? "yes" : "no",
          });

          await loadStatus();
          await loadEvents({ reset: true });
        } catch (error) {
          setOutput("#auth-output", {
            ok: false,
            error: error.message,
          });
        }
      });
    }

    if (checkButton) {
      checkButton.addEventListener("click", async () => {
        if (!authState.sessionId) {
          setOutput("#auth-output", "Login first to get a session id.");
          return;
        }

        try {
          setOutput("#auth-output", "Checking session…");

          const result = await postJson("/api/auth/check", {
            session_id: authState.sessionId,
          });

          setOutput("#auth-output", result);

          await loadStatus();
          await loadEvents({ reset: true });
        } catch (error) {
          setOutput("#auth-output", {
            ok: false,
            error: error.message,
          });
        }
      });
    }

    if (logoutButton) {
      logoutButton.addEventListener("click", async () => {
        if (!authState.sessionId) {
          setOutput("#auth-output", "Login first to get a session id.");
          return;
        }

        try {
          setOutput("#auth-output", "Logging out…");

          const result = await postJson("/api/auth/logout", {
            session_id: authState.sessionId,
          });

          authState.sessionId = "";

          setOutput("#auth-output", result);

          await loadStatus();
          await loadEvents({ reset: true });
        } catch (error) {
          setOutput("#auth-output", {
            ok: false,
            error: error.message,
          });
        }
      });
    }

    if (deleteButton) {
      deleteButton.addEventListener("click", async () => {
        if (!authState.sessionId) {
          setOutput("#auth-output", "Login first to get a session id.");
          return;
        }

        const confirmed = window.confirm(
          "Delete this diagnostic account? This cannot be undone.",
        );

        if (!confirmed) {
          return;
        }

        try {
          setOutput("#auth-output", "Deleting account…");

          const result = await deleteJson("/api/auth/account", {
            session_id: authState.sessionId,
          });

          authState.sessionId = "";

          setOutput("#auth-output", result);

          await loadStatus();
          await loadEvents({ reset: true });
        } catch (error) {
          setOutput("#auth-output", {
            ok: false,
            error: error.message,
          });
        }
      });
    }
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
      const host =
        window.location.protocol === "https:"
          ? window.location.host
          : `${window.location.hostname || "localhost"}:9090`;
      const url = `${protocol}//${host}/ws`;

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
          "Check that WebSocket is reachable at /ws through the current host.",
        );
      });
    });
  };

  const init = async () => {
    setupRefreshButton();
    setupEventsPagination();
    setupKvForm();
    setupJobForm();
    setupAuthForm();
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

/* ============================================================
 * Home page — live terminal (index.html only)
 * Self-contained IIFE. Activates only when #ph-terminal-body
 * exists, so it stays inert on the status dashboard.
 * ============================================================ */
(() => {
  "use strict";

  const term = document.getElementById("ph-terminal-body");
  if (!term) return;

  const prefersReduced = window.matchMedia(
    "(prefers-reduced-motion: reduce)",
  ).matches;

  const esc = (s) =>
    String(s)
      .replaceAll("&", "&amp;")
      .replaceAll("<", "&lt;")
      .replaceAll(">", "&gt;");

  const highlightJson = (json) => {
    return esc(json).replace(
      /("(\\.|[^"\\])*"(\s*:)?|\b(true|false|null)\b|-?\d+(\.\d+)?([eE][+-]?\d+)?)/g,
      (match) => {
        let cls = "ph-j-num";
        if (/^"/.test(match)) {
          cls = /:$/.test(match) ? "ph-j-key" : "ph-j-str";
        } else if (/true|false/.test(match)) {
          cls = "ph-j-bool";
        } else if (/null/.test(match)) {
          cls = "ph-j-punct";
        }
        return `<span class="${cls}">${match}</span>`;
      },
    );
  };

  const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

  const CMD = "curl -s https://pico.vixcpp.com/api/status | jq";

  const renderPrompt = (typed, withCursor) =>
    `<span class="ph-Terminal__prompt">$ </span>` +
    `<span class="ph-Terminal__cmd">${esc(typed)}</span>` +
    (withCursor ? `<span class="ph-Terminal__cursor"></span>` : "");

  const typeCommand = async () => {
    if (prefersReduced) {
      term.innerHTML = renderPrompt(CMD, false) + "\n\n";
      return;
    }
    for (let i = 0; i <= CMD.length; i += 1) {
      term.innerHTML = renderPrompt(CMD.slice(0, i), true);
      await sleep(18 + Math.random() * 24);
    }
    term.innerHTML = renderPrompt(CMD, false) + "\n\n";
  };

  const shape = (data) => {
    const out = {
      app: data?.app ?? "pico",
      runtime: data?.runtime ?? "Vix.cpp",
      status: data?.status ?? "running",
      uptime_seconds: data?.uptime_seconds ?? 0,
      boot_count: data?.boot_count ?? 0,
      http_requests: data?.http_requests ?? 0,
      ws_messages: data?.ws_messages ?? 0,
      events: { total: data?.events?.total ?? 0 },
    };
    const tp = data?.threadpool;
    if (tp) {
      out.threadpool = {
        running: tp.running ?? true,
        workers: tp?.metrics?.workers ?? 0,
        completed: tp?.stats?.completed ?? 0,
      };
    }
    return out;
  };

  const SAMPLE = {
    app: "pico",
    runtime: "Vix.cpp",
    status: "running",
    uptime_seconds: 4215,
    boot_count: 7,
    http_requests: 1284,
    ws_messages: 42,
    events: { total: 58 },
    threadpool: { running: true, workers: 8, completed: 134 },
  };

  const streamJson = async (obj, { live }) => {
    const base = term.innerHTML;
    const json = JSON.stringify(obj, null, 2);
    const highlighted = highlightJson(json);

    if (prefersReduced) {
      term.innerHTML = base + highlighted;
      return;
    }

    const lines = highlighted.split("\n");
    let shown = "";
    for (let i = 0; i < lines.length; i += 1) {
      shown += (i ? "\n" : "") + lines[i];
      term.innerHTML = base + shown;
      term.scrollTop = term.scrollHeight;
      await sleep(28);
    }

    if (!live) {
      term.innerHTML =
        base +
        shown +
        `\n\n<span class="ph-Terminal__prompt"># offline sample — start Pico to see live data</span>`;
    }
  };

  const run = async () => {
    await typeCommand();
    try {
      const res = await fetch("/api/status", {
        headers: { Accept: "application/json" },
      });
      if (!res.ok) throw new Error(String(res.status));
      const data = await res.json();
      await streamJson(shape(data), { live: true });
    } catch {
      await streamJson(SAMPLE, { live: false });
    }
  };

  const boot = () => {
    if ("IntersectionObserver" in window) {
      const io = new IntersectionObserver(
        (entries, obs) => {
          entries.forEach((e) => {
            if (e.isIntersecting) {
              obs.disconnect();
              run();
            }
          });
        },
        { threshold: 0.3 },
      );
      io.observe(term);
    } else {
      run();
    }
  };

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", boot);
  } else {
    boot();
  }
})();
