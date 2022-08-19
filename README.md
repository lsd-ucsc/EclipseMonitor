# Eclipse Monitor

An implementation of eclipse monitor in C++ introduced in the paper
*[Total Eclipse of the Enclave: Detecting Eclipse Attacks From Inside TEEs](https://doi.org/10.1109/ICBC51069.2021.9461081)*

## Workflow Diagram

![Workflow Diagram](./docs/flow_diagram.svg)

## Code Status
- [![Unit Tests](https://github.com/zhenghaven/EclipseMonitor/actions/workflows/unit-tests.yaml/badge.svg?branch=main)](https://github.com/zhenghaven/EclipseMonitor/actions/workflows/unit-tests.yaml)
	- Testing environments
		- OS: `ubuntu-latest`, `windows-latest`, `macos-latest`
		- C++ std: `11`, `20` (by setting CXX_STANDARD in CMake)
