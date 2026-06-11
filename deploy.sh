#!/usr/bin/env bash
set -euo pipefail

APP_DIR="/home/gaspard/vixcpp/pico"
VIX="/home/gaspard/.local/bin/vix"

cd "$APP_DIR"

echo "==> Pull latest code"
git fetch origin main
git reset --hard origin/main

echo "==> Update and install dependencies"
"$VIX" registry sync
"$VIX" update
"$VIX" install

echo "==> Build Pico"
"$VIX" build --build-target all -v

echo "==> Restart service"
sudo -n systemctl restart pico.service

echo "==> Check service"
sudo -n systemctl status pico.service --no-pager

echo "==> Waiting for Pico HTTP health"
for i in {1..20}; do
  if curl -fsS http://127.0.0.1:8080/health >/dev/null; then
    echo "Pico local health is ready"
    break
  fi

  echo "Waiting for Pico... attempt $i/20"
  sleep 1
done

echo "==> Health checks"
"$VIX" health local
"$VIX" health public

echo "==> Production doctor"
sudo -n "$VIX" doctor production

echo "==> Pico deployed successfully"
