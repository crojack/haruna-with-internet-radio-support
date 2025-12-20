#!/bin/bash

# Replace all KFileMetaData includes with the correct nested path
find . -type f \( -name "*.cpp" -o -name "*.h" \) -print0 | while IFS= read -r -d '' file; do
  echo "Fixing includes in $file"
  sed -i 's|#include <KFileMetaData/Properties>|#include <KFileMetaData/KFileMetaData/Properties>|g' "$file"
done

echo "✅ All KFileMetaData includes updated."

