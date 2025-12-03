# DevOps Taskboard

This project is a small C++ backend service used as a demo application for the **Modern Practices in DevOps** course final project.

The main focus is not on complex business logic, but on demonstrating a complete **CI/CD pipeline**, containerization, Kubernetes deployment, and basic security practices around the service.

---

## Project Goals

The project demonstrates:

- Building and testing a C++ service with **CMake**
- Packaging the service as a **Docker image**
- Publishing images to **GitHub Container Registry (GHCR)**
- Implementing **Continuous Integration (CI)** with GitHub Actions:
  - Build, unit tests, Docker build
- Implementing **Continuous Delivery (CD)** with GitHub Actions:
  - Build & push Docker image
  - Deploy to a **Kubernetes** cluster (ephemeral kind cluster in CI)
  - Automatic `/health` smoke test inside the cluster
- Applying key DevOps practices:
  - Source control (Git + GitHub)
  - Branching & pipelines
  - Security / SAST (**CodeQL** analysis for C++)
  - Infrastructure as Code (**Kubernetes manifests**)

---

## Service Overview

Currently the backend exposes a single HTTP endpoint:

- `GET /health` – returns a small JSON payload indicating that the service is alive, e.g.:

  ```json
  {
    "status": "ok",
    "service": "taskboard-backend"
  }

