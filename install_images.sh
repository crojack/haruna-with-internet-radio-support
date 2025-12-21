#!/bin/bash

# install_images.sh - Install Haruna Internet Radio images
# This script copies the images directory to the user's local Haruna data directory

set -e  # Exit on error

# Color codes for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Define paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="${SCRIPT_DIR}/images"
TARGET_BASE="${HOME}/.local/share/KDE"
TARGET_DIR="${TARGET_BASE}/haruna"
TARGET_IMAGES="${TARGET_DIR}/images"

echo -e "${GREEN}Haruna Internet Radio - Image Installation Script${NC}"
echo "=================================================="
echo ""

# Check if source images directory exists
if [ ! -d "${SOURCE_DIR}" ]; then
    echo -e "${RED}Error: Source images directory not found at: ${SOURCE_DIR}${NC}"
    echo "Please run this script from the root of the haruna-with-internet-radio-support directory."
    exit 1
fi

# Create target directory structure if it doesn't exist
echo -e "${YELLOW}Checking target directories...${NC}"
if [ ! -d "${TARGET_BASE}" ]; then
    echo "Creating: ${TARGET_BASE}"
    mkdir -p "${TARGET_BASE}"
fi

if [ ! -d "${TARGET_DIR}" ]; then
    echo "Creating: ${TARGET_DIR}"
    mkdir -p "${TARGET_DIR}"
fi

# Backup existing images directory if it exists
if [ -d "${TARGET_IMAGES}" ]; then
    BACKUP_DIR="${TARGET_IMAGES}.backup.$(date +%Y%m%d_%H%M%S)"
    echo -e "${YELLOW}Existing images directory found. Creating backup at:${NC}"
    echo "  ${BACKUP_DIR}"
    mv "${TARGET_IMAGES}" "${BACKUP_DIR}"
fi

# Copy images directory
echo -e "${YELLOW}Copying images directory...${NC}"
cp -r "${SOURCE_DIR}" "${TARGET_IMAGES}"

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Images directory successfully installed to:${NC}"
    echo "  ${TARGET_IMAGES}"
    echo ""
    echo "Directory structure:"
    tree -L 2 "${TARGET_IMAGES}" 2>/dev/null || find "${TARGET_IMAGES}" -type d
    echo ""
    echo -e "${GREEN}Installation complete!${NC}"
else
    echo -e "${RED}✗ Error copying images directory${NC}"
    exit 1
fi

# Set proper permissions
echo -e "${YELLOW}Setting permissions...${NC}"
chmod -R u+rw "${TARGET_IMAGES}"
echo -e "${GREEN}✓ Permissions set${NC}"

echo ""
echo -e "${GREEN}All done! You can now run Haruna with Internet Radio support.${NC}"

exit 0
