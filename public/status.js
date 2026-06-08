async function refreshStatus() {
  const line = document.getElementById("status-line");
  if (!line) return;

  try {
    const response = await fetch("/api/health", {
      headers: { Accept: "application/json" },
    });

    if (!response.ok) {
      line.textContent = "Service responded with HTTP " + response.status;
      return;
    }

    line.textContent = "Service is healthy.";
  } catch (error) {
    line.textContent = "Service health check failed.";
  }
}

refreshStatus();
