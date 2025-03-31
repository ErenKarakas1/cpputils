cppcheck_config=".cppcheck"
cppcheck_cache=".cache/cppcheck"
build_dir="./build/debug"

if ! builtin type -P cppcheck &>/dev/null; then
    echo "cppcheck is not installed. Please install it before running this script."
    exit 1
fi

mkdir -p "${cppcheck_cache}"

cppcheck --project="${build_dir}/compile_commands.json" --suppressions-list="${cppcheck_config}" --cppcheck-build-dir="${cppcheck_cache}" --check-level=exhaustive
