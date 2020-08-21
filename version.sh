#!/bin/bash

# ============================================================================
# File:
#   version.sh
#   - get version of repository and generate the file version.h
# ============================================================================

# get version of remote and local repository

shell_dir=""
if [ ! -n "$1" ]; then
    shell_dir="."
else
    shell_dir=$1
fi

VER_R=`git rev-list origin/master | sort | wc -l | gawk '{print $1}'`
VER_L=`git rev-list HEAD | sort | wc -l | gawk '{print $1}'`
VER_SHA1=`git log -n 1 | head -n 1 | cut -d ' ' -f 2`

major_version="1"
minor_version="2"
type_version="release"

# generate the file version.h
echo "// ==========================================================================="  > $shell_dir/version.h
echo "// version.h"                                                                   >> $shell_dir/version.h
echo "// - collection of version numbers"                                             >> $shell_dir/version.h
echo "// ===========================================================================" >> $shell_dir/version.h
echo ""                                                                               >> $shell_dir/version.h
echo "#ifndef __VERSION_H__"                                                          >> $shell_dir/version.h
echo "#define __VERSION_H__"                                                          >> $shell_dir/version.h
echo ""                                                                               >> $shell_dir/version.h
echo "#define VER_MAJOR  $major_version                // major version number"       >> $shell_dir/version.h
echo "#define VER_MINOR  $minor_version                // minor version number"       >> $shell_dir/version.h
echo "#define VER_BUILD  $VER_L             // build number"                          >> $shell_dir/version.h
echo ""                                                                               >> $shell_dir/version.h
echo "#define VERSION_TYPE \"$type_version\""                                         >> $shell_dir/version.h
echo "#define VERSION_STR  \"$major_version.$minor_version.$VER_L\""                  >> $shell_dir/version.h
echo "#define VERSION_SHA1 \"$VER_SHA1\""                                             >> $shell_dir/version.h
echo ""                                                                               >> $shell_dir/version.h
echo "#endif // __VERSION_H__"                                                        >> $shell_dir/version.h

# show version informations
echo "  "
echo "                      GIT VERSION TOOLS"
echo "                    ====================="
echo "  "
echo "  get the code version number of remote & local repository."
echo "  "
echo "       remote: $VER_R"
echo "        local: $VER_L"
echo "        SHA-1: $VER_SHA1"
echo "  "
echo "  remote version $VER_L is added to file version.h, such as:"
echo "  "
echo "  #define VER_BUILD    $VER_L"
echo "  "
echo "  #define VERSION_SHA1 $VER_SHA1"
echo "  "
