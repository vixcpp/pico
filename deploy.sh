#!/usr/bin/env bash
set -euo pipefail

APP_DIR="/home/gaspard/vixcpp/pico"
VIX="/home/gaspard/.local/bin/vix"

cd "$APP_DIR"

echo "==> Pull latest code"
git fetch origin main
git reset --hard origin/main

echo "==> Install dependencies"
"$VIX" registry sync
"$VIX" install

echo "==> Build Pico"
"$VIX" build --build-target all -v

echo "==> Restart service"
sudo -n systemctl restart pico.service

echo "==> Check service"
sudo -n systemctl status pico.service --no-pager

echo "==> Health checks"
"$VIX" health local
"$VIX" health public

echo "==> Production doctor"
sudo -n "$VIX" doctor production

echo "==> Pico deployed successfully"
