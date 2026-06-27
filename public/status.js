/*
 * Pico Status — dashboard shell
 * Tabs, kernel activity indicator, and status bar.
 * Form/endpoint wiring lives in app.js (unchanged); this file only drives
 * the IDE chrome around it.
 */
(() => {
  "use strict";

  const $ = (sel) => document.querySelector(sel);
  const $$ = (sel) => Array.from(document.querySelectorAll(sel));

  const shell = $("#ps-shell");

  /* ----------------------------------------------------------
     Kernel activity indicator (Idle / Busy / Error)
     Wraps window.fetch so any in-flight request flips the dot
     to "busy", and a failed request flips it to "error".
     ---------------------------------------------------------- */
  const kernel = {
    inflight: 0,
    setText(state) {
      const label =
        state === "busy" ? "Busy" : state === "error" ? "Error" : "Idle";
      const k = $("#kernel-text");
      const sb = $("#statusbar-kernel");
      if (k) k.textContent = label;
      if (sb) sb.textContent = label;
    },
    set(state) {
      if (!shell) return;
      shell.setAttribute("data-kernel", state);
      this.setText(state);
    },
    begin() {
      this.inflight += 1;
      this.set("busy");
    },
    end(failed) {
      this.inflight = Math.max(0, this.inflight - 1);
      if (failed) {
        this.set("error");
        return;
      }
      if (this.inflight === 0) this.set("idle");
    },
  };

  const nativeFetch = window.fetch.bind(window);
  window.fetch = (...args) => {
    kernel.begin();
    return nativeFetch(...args).then(
      (res) => {
        kernel.end(!res.ok);
        return res;
      },
      (err) => {
        kernel.end(true);
        throw err;
      },
    );
  };

  /* ----------------------------------------------------------
     Status bar — fed from /api/status, refreshed on a light timer
     ---------------------------------------------------------- */
  const formatUptime = (seconds) => {
    const s = Number(seconds);
    if (!Number.isFinite(s)) return "—";
    if (s < 60) return `${Math.floor(s)}s`;
    if (s < 3600) return `${Math.floor(s / 60)}m ${Math.floor(s % 60)}s`;
    const h = Math.floor(s / 3600);
    const m = Math.floor((s % 3600) / 60);
    return `${h}h ${m}m`;
  };

  const refreshStatusBar = async () => {
    try {
      const res = await nativeFetch("/api/status", {
        headers: { Accept: "application/json" },
      });
      if (!res.ok) return;
      const data = await res.json();

      const app = $("#statusbar-app");
      const uptime = $("#statusbar-uptime");
      const events = $("#statusbar-events");
      const badge = $("#events-badge");

      if (app && data?.app) app.textContent = data.app;
      if (uptime)
        uptime.textContent = `uptime ${formatUptime(data?.uptime_seconds)}`;

      const total = data?.events?.total;
      if (events) events.textContent = `${total ?? 0} events`;
      if (badge && total != null) badge.textContent = String(total);
    } catch {
      /* status bar is best-effort; ignore failures */
    }
  };

  const setHost = () => {
    const host = $("#statusbar-host");
    if (host) host.textContent = window.location.host || "localhost";
  };

  /* ----------------------------------------------------------
     Tabs
     ---------------------------------------------------------- */
  const TAB_KEY = "pico:status:tab";

  const activateTab = (name, { focus = false } = {}) => {
    const tabs = $$(".ps-Tab");
    const panels = $$(".ps-Panel");
    let matched = false;

    tabs.forEach((tab) => {
      const isActive = tab.dataset.tab === name;
      tab.classList.toggle("is-active", isActive);
      tab.setAttribute("aria-selected", isActive ? "true" : "false");
      tab.tabIndex = isActive ? 0 : -1;
      if (isActive) {
        matched = true;
        if (focus) tab.focus();
      }
    });

    if (!matched) return false;

    panels.forEach((panel) => {
      panel.hidden = panel.id !== `panel-${name}`;
    });

    try {
      sessionStorage.setItem(TAB_KEY, name);
    } catch {
      /* private mode: ignore */
    }

    // Scroll the body back to the top on tab change.
    const body = $(".ps-Body");
    if (body) body.scrollTop = 0;

    return true;
  };

  const setupTabs = () => {
    const tabs = $$(".ps-Tab");

    tabs.forEach((tab) => {
      tab.addEventListener("click", () => activateTab(tab.dataset.tab));
    });

    // Roving arrow-key navigation across the tablist.
    const tablist = $("#ps-tabs");
    if (tablist) {
      tablist.addEventListener("keydown", (e) => {
        if (e.key !== "ArrowRight" && e.key !== "ArrowLeft") return;
        e.preventDefault();
        const order = tabs.map((t) => t.dataset.tab);
        const current = order.findIndex((n) =>
          $(`#tab-${n}`)?.classList.contains("is-active"),
        );
        const dir = e.key === "ArrowRight" ? 1 : -1;
        const next = (current + dir + order.length) % order.length;
        activateTab(order[next], { focus: true });
      });
    }

    // Restore last tab, or honor a #hash deep-link.
    const hash = window.location.hash.replace("#", "");
    let initial = "overview";
    try {
      initial = sessionStorage.getItem(TAB_KEY) || initial;
    } catch {
      /* ignore */
    }
    if (hash && $(`#tab-${hash}`)) initial = hash;
    activateTab(initial);
  };

  /* ----------------------------------------------------------
     Boot
     ---------------------------------------------------------- */
  const start = () => {
    setupTabs();
    setHost();
    refreshStatusBar();
    // Keep the status bar fresh without hammering the runtime.
    setInterval(refreshStatusBar, 10000);

    // Re-sync the status bar whenever the user hits Refresh.
    const refresh = $("#refresh-status");
    if (refresh) {
      refresh.addEventListener("click", () => {
        setTimeout(refreshStatusBar, 250);
      });
    }
  };

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", start);
  } else {
    start();
  }
})();
