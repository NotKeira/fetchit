#!/bin/bash

# Install FetchIt for Linux

INSTALL_DIR="$HOME/.local/bin"

mkdir -p "$INSTALL_DIR"

cp "../fetchit" "$INSTALL_DIR/fetchit"

chmod +x "$INSTALL_DIR/fetchit"

if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
    CURRENT_SHELL=$(basename "$SHELL")
    RC_FILE=""
    case "$CURRENT_SHELL" in
        "bash")
            RC_FILE="$HOME/.bashrc"
        ;;
        "zsh")
            RC_FILE="$HOME/.zshrc"
        ;;
        "fish")
            RC_FILE="$HOME/.config/fish/config.fish"
        ;;
        *)
            echo "Unsupported shell: $CURRENT_SHELL"
            exit 1
        ;;
    esac
    echo "Adding $INSTALL_DIR to PATH in $RC_FILE"
    if [ "$CURRENT_SHELL" = "fish" ]; then
        echo 'set -gx PATH "$HOME/.local/bin" $PATH' >> "$RC_FILE"
    else
        echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$RC_FILE"
    fi
    source "$RC_FILE"
    echo "Please restart your terminal or run: source $RC_FILE"
fi
clear
echo -e "\e[32m[FetchIt]\e[0m Installation complete. You can now run \e[33m\`fetchit\`\e[0m anywhere."