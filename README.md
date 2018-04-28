# DirectX11-Framework

This is a simple framework for DirectX11.

## Usage

1. Derive a class from D3D11App.
2. Implement the pure virtual functions below and other virtual functions that you needed.

```cpp
	virtual void UpdateScene(float DeltaTime) = 0;
	virtual void DrawScene() = 0;
```

The program is running as:

Init -> Loop(UpdateScene -> DrawScene)

Main.cpp is an example of how to use this simple framework.
