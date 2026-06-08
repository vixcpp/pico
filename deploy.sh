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
"$VIX" service restart

echo "==> Check service"
"$VIX" service status

echo "==> Health checks"
"$VIX" health local
"$VIX" health public

echo "==> Production doctor"
sudo "$VIX" doctor production

echo "==> Pico deployed successfully"
