#!/bin/bash

# Install FetchIt for Linux

INSTALL_DIR="/usr/local/bin"

echo "Installing FetchIt to $INSTALL_DIR..."
sudo mkdir -p "$INSTALL_DIR"
sudo cp "../fetchit" "$INSTALL_DIR/fetchit"
sudo chmod +x "$INSTALL_DIR/fetchit"

echo -e "\e[32m[FetchIt]\e[0m Installation complete. You can now run \e[33m\`fetchit\`\e[0m from anywhere."