CREATE TABLE IF NOT EXISTS events (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  source TEXT NOT NULL,
  type TEXT NOT NULL,
  payload TEXT NOT NULL DEFAULT '{}',
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_events_source
ON events (source);

CREATE INDEX IF NOT EXISTS idx_events_type
ON events (type);

CREATE INDEX IF NOT EXISTS idx_events_created_at
ON events (created_at);
