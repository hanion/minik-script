#!/bin/bash

print_usage() {
	echo
	echo "Usage: $0 [clean] [build] [test] [config]"
	echo "    clean: Clean build"
	echo "    build: Perform the build only, without running the executable"
	echo "    test: Run tests"
	echo "    config: debug  | release | min"
}

clean_build() {
	print_job "Cleaning build:"
	print_job "    rm -r ./build/"
	rm -r ./build/
	print_success "Cleaning done"
	configure_first=true
}

configure_project() {
	print_job "Configuring project:"
	print_job "    cmake -DCMAKE_BUILD_TYPE:STRING=$cmake_config -S./ -B./build -G Ninja"

	cmake --no-warn-unused-cli \
		-DCMAKE_BUILD_TYPE:STRING=$cmake_config \
		-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
		-DCMAKE_C_COMPILER:FILEPATH=gcc \
		-DCMAKE_CXX_COMPILER:FILEPATH=g++ \
		-DCMAKE_CXX_FLAGS="-std=c++17" \
		-S./ -B./build -G Ninja

	if [ $? -eq 0 ]; then
		print_success "Configuring done"
	else
		print_error "Configuring failed"
		exit 1
	fi
}

build_project() {
	print_job "Building project:"
	print_job "    cmake --build ./build --target all -- -j $(nproc)"

	start_time_ms=$(date +%s%3N)
	cmake --build ./build --target all -- -j $(nproc)
	build_success=$?
	end_time_ms=$(date +%s%3N)

	if [ $build_success -eq 0 ]; then
		print_success "Building done in $(elapsed_time_ms $start_time_ms $end_time_ms)"
	else
		print_error "Building failed"
		exit 1
	fi
}

run_executable() {
	print_job "Running:"
	print_job "    ./"$target""
	cd "$build_dir"
	./"$target"
}

run_tests() {
	print_job "Running Tests:"
	print_job "    ./"$target" --run-tests"
	cd "$build_dir"
	./"$target" --run-tests
}

elapsed_time_ms() {
	local start=$1
	local end=$2
	local elapsed=$(( end - start ))
	echo "$((elapsed / 1000))s $((elapsed % 1000))ms"
}

supports_color() {
	[[ -t 1 && $(command -v tput) && $(tput colors) -ge 8 ]]
}

print_colored() {
	local color_code=$1
	local message=$2
	if supports_color; then
		echo -e "\033[${color_code}m${message}\033[0m"
	else
		echo "$message"
	fi
}

print_success() { print_colored "32" "$1"; }
print_error() { print_colored "91" "$1"; }
print_job() { print_colored "96" "$1"; }





clean_build=false
build_only=false
run_tests=false
configure_first=false
config="debug"
target="minik-script"

while [[ $# -gt 0 ]]; do
	key="$1"

	case $key in
		clean)
			clean_build=true
			shift
			;;
		build)
			build_only=true
			shift
			;;
		test)
			run_tests=true
			shift
			;;
		debug|release|min)
			config="$1"
			configure_first=true
			shift
			;;
		*)
			print_error "Unknown option: $key"
			print_usage
			exit 1
			;;
	esac
done

case "$target" in
	minik-script)
		build_dir="./build"
		;;
	*)
		echo "Invalid target: $target"
		print_usage
		exit 1
		;;
esac

case "$config" in
	debug)
		cmake_config="Debug"
		;;
	release)
		cmake_config="Release"
		;;
	min)
		cmake_config="MinSizeRel"
		;;
	*)
		echo "Invalid configuration: $config"
		print_usage
		exit 1
		;;
esac


if [ ! -d "./build" ]; then
	configure_first=true
fi


if [[ "$clean_build" == true ]]; then
	clean_build
fi

if [[ "$configure_first" == true ]]; then
	configure_project
fi

build_project

if [[ "$build_only" != true ]]; then
	if [ $build_success -eq 0 ]; then
		if [[ "$run_tests" == true ]]; then
			run_tests
		else
			run_executable
		fi
	fi
fi


