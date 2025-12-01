# DevOps Taskboard

This project is a small C++ backend service used as a demo application for a **Modern Practices in DevOps** course final project.

The main goals of the project are:

- Demonstrate a full CI/CD pipeline with GitHub Actions
- Build and ship a C++ service as a Docker image
- Deploy the service to a Kubernetes cluster
- Apply DevOps practices such as:
  - Source control and branching strategies
  - Continuous Integration (build, tests, static analysis)
  - Continuous Delivery (automatic deploy to Kubernetes)
  - Security scanning (SAST, container image scanning, secret management)
  - Infrastructure as Code (Kubernetes manifests, optionally Terraform)
  - Database schema migrations (Flyway)

## Service Overview

For now, the backend exposes a single HTTP endpoint:

- `GET /health` – returns a simple JSON response indicating that the service is alive.

The plan is to evolve this into a small "TaskBoard" API (tasks, statuses, priorities), but the **DevOps pipeline and infrastructure** are the main focus of the project.

## Build (local)

The project uses CMake and is intended to be built on Linux (for Docker/Kubernetes compatibility):

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
