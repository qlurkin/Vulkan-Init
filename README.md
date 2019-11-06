# Vulkan Init

## Philosophy

- Use Vulkan C++ API
- Not Hide Vulkan API
- Provide Helpers with sane defaults under the vkh namespace
- Keep compatibility with standard Vulkan API
- Vulkan should stay the only dependency

## build

Creating project files:

```
mkdir build
cd build
cmake ..
```

building *(in the `build` folder)*:

```
cmake --build .
```