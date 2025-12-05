#!/bin/bash
#=============================================================================
# HTTPS Test Setup Helper
# Creates certificates and starts test server
#=============================================================================

OPENSSL_BIN="/opt/homebrew/opt/openssl@3/bin/openssl"

echo "=== HTTPS Test Setup ==="
echo ""

# Generate test certificates if they don't exist
if [ ! -f server.key ] || [ ! -f server.crt ]; then
    echo "Generating test certificates..."
    $OPENSSL_BIN req -x509 -newkey rsa:2048 -keyout server.key -out server.crt \
        -days 365 -nodes -subj "/CN=localhost"
    echo "✓ Certificates created"
fi

echo ""
echo "Starting HTTPS server on port 8443..."
echo "Keep this terminal open!"
echo ""

# Start server
$OPENSSL_BIN s_server -cert server.crt -key server.key -accept 8443 -tls1_3 -www
