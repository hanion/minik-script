# minik-script

A lightweight, interpreted scripting language. Inspired by languages like Odin and Jai, `minik-script` features a clean, minimal syntax that makes it easy to write scripts.

```odin

// variables
number := 3.14;
text := "hello";
print(number, text);

// control flow
for i := 0; i < 5; ++i {
    if i == 2 {
        print("found 2!");
        break;
    }
}

// functions
add :: (a, b) {
    return a + b;
}
sum := add(3, 4);
print("Sum:", sum);


// defer
{
    defer print("this will print last");
    print("this will print first");
}

```

## Building

```bash

# Clone the repo
git clone https://github.com/hanion/minik-script.git
cd minik-script

# Build with build.sh
./build.sh 

# or build with cmake
mkdir build && cd build
cmake ..
cmake --build .

```

## Running

```bash

# Run the interpreter with a script
./minik-script script.mn

```
