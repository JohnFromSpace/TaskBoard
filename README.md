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

The implementation is intentionally minimal so that the main focus is on the DevOps pipeline and infrastructure (Docker, Kubernetes, CI/CD, security).

---

## Technologies

* **Language:** C++17
* **Build system:** CMake
* **Tests:** Custom test runner + `ctest`
* **Containers:** Docker (multi-stage build)
* **Registry:** GitHub Container Registry (**GHCR**)
* **Kubernetes:** kind (ephemeral cluster inside GitHub Actions) + K8s YAML manifests
* **CI/CD:** GitHub Actions (`.github/workflows/ci.yml`, `.github/workflows/cd.yml`)
* **Security / SAST:** GitHub **CodeQL** scanning for C++

---

## Repository Structure

High-level layout:

```bash
.
├─ backend/
│  ├─ include/
│  │  └─ http_responses.h          # Common HTTP response helpers
│  ├─ src/
│  │  ├─ main.cpp                  # Socket-based HTTP server + /health
│  │  └─ http_responses.cpp        # Implementation of HTTP responses
│  ├─ tests/
│  │  └─ http_responses_tests.cpp  # Unit tests for HTTP response logic
│  └─ CMakeLists.txt
│
├─ k8s/
│  ├─ namespace.yaml               # Namespace: taskboard
│  ├─ deployment.yaml              # Deployment: taskboard-backend
│  └─ service.yaml                 # Service: taskboard-backend (ClusterIP)
│
├─ .github/
│  └─ workflows/
│     ├─ ci.yml                    # CI: build + tests + docker build
│     └─ cd.yml                    # CD: build + push + deploy to Kubernetes (kind)
│
├─ CMakeLists.txt                  # Root CMake project
├─ Dockerfile                      # Multi-stage Docker build
├─ .dockerignore                   # Ignore build artifacts, .git, etc.
└─ README.md
```

---

## Local Build and Run

### Option 1: CMake on Linux / WSL / MinGW

From the project root:

```bash
mkdir -p build
cd build
cmake ..
cmake --build . --config Release
```

This produces (paths depend on your CMake generator):

* `backend/taskboard_backend` – the server executable
* `backend/taskboard_tests` – unit tests binary

Run the backend:

```bash
./backend/taskboard_backend
# The server listens on port 5000 by default
```

Test the `/health` endpoint:

```bash
curl -v http://localhost:5000/health
```

You should receive a JSON response similar to:

```json
{"status":"ok","service":"taskboard-backend"}
```

### Option 2: Visual Studio (Windows)

1. Open Visual Studio and use **“Open a local folder”** pointing to the repository root.
2. Visual Studio will detect the CMake project automatically.
3. Use **Build → Build All** (`Ctrl+Shift+B`) to build:

   * `taskboard_core` static library
   * `taskboard_backend` executable
   * `taskboard_tests` executable
4. Select `taskboard_backend` as the startup target and run (Ctrl+F5).
5. Use `curl` or a browser to call `http://localhost:5000/health`.

---

## Unit Tests

Unit tests are located in:

* `backend/tests/http_responses_tests.cpp`

They validate:

* Correct status lines:

  * `HTTP/1.1 200 OK` for `/health`
  * `HTTP/1.1 404 Not Found` for unknown paths
* Presence of `Content-Type: application/json`
* Correct JSON bodies for the responses

Run tests locally via CTest:

```bash
cd build
ctest --output-on-failure
```

The CI pipeline (`ci.yml`) runs the same `ctest` command on every push / pull request.

---

## Docker

### Dockerfile

The root `Dockerfile` is a multi-stage build:

1. **Builder stage**

   * Base image: `debian:bookworm-slim`
   * Installs `build-essential` and `cmake`
   * Configures and builds the C++ project with CMake

2. **Runtime stage**

   * Base image: `debian:bookworm-slim`
   * Copies only the compiled `taskboard_backend` binary from the builder stage
   * Exposes port `5000`
   * Starts the server via:

     ```bash
     ./taskboard_backend
     ```

### Local Docker Build (optional)

If you have Docker installed locally, you can build and run the image:

```bash
docker build -t taskboard-backend:local .
docker run --rm -p 5000:5000 taskboard-backend:local

curl -v http://localhost:5000/health
```

---

## Kubernetes

Kubernetes manifests are located in the `k8s/` directory:

* `namespace.yaml` – defines the `taskboard` Namespace.
* `deployment.yaml` – defines a `Deployment`:

  * Name: `taskboard-backend`
  * Namespace: `taskboard`
  * 2 replicas
  * Container image: `ghcr.io/johnfromspace/taskboard-backend:latest`
  * Container port: `5000`
  * `readinessProbe` and `livenessProbe` using the `/health` endpoint
* `service.yaml` – defines a `ClusterIP` `Service`:

  * Name: `taskboard-backend`
  * Namespace: `taskboard`
  * Service port: `80`
  * Target port: `5000`
  * Selector: `app: taskboard-backend`

### Deploy with kind (as in the CD pipeline)

The CD pipeline uses **kind** (Kubernetes-in-Docker) inside GitHub Actions to create an ephemeral Kubernetes cluster for each run. The basic idea is:

1. Create a kind cluster in the CI runner.
2. Load the Docker image into the kind nodes.
3. Apply the Kubernetes manifests.
4. Wait for the rollout to finish.
5. Port-forward the service and call `/health`.

This provides a real Kubernetes deployment and smoke test without requiring a persistent external cluster or local Docker Desktop.

---

## CI Pipeline – `.github/workflows/ci.yml`

The **CI workflow** is responsible for:

* Ensuring that the C++ code builds successfully.
* Running unit tests.
* Validating that the Dockerfile can build.

Key steps:

1. **Checkout**

   ```yaml
   - uses: actions/checkout@v4
   ```

2. **Install build tools**

   ```yaml
   - run: |
       sudo apt-get update
       sudo apt-get install -y --no-install-recommends \
         build-essential \
         cmake
   ```

3. **Configure & build with CMake**

   ```yaml
   - run: |
       mkdir -p build
       cd build
       cmake ..
       cmake --build . --config Release
   ```

4. **Run tests**

   ```yaml
   - run: |
       cd build
       ctest --output-on-failure
   ```

5. **Docker build sanity check**

   ```yaml
   - run: |
       docker build -t taskboard-backend:ci .
   ```

Permissions are restricted to the minimum needed:

```yaml
permissions:
  contents: read
```

---

## CD Pipeline – `.github/workflows/cd.yml`

The **CD workflow** handles:

* Building and pushing the Docker image to GHCR.
* Deploying the application to a Kubernetes cluster (kind) inside GitHub Actions.
* Running a smoke test against `/health`.

High-level steps:

1. **Checkout repository**

2. **Set up Docker Buildx**

3. **Log in to GHCR** using `github.token`

4. **Build & push Docker image**

   * Image tag format:

     ```text
     ghcr.io/<owner>/taskboard-backend:{sha, latest}
     ```

5. **Create a kind cluster** using `helm/kind-action`

6. **Load the image into kind** to avoid pulling from the network

7. **Apply Kubernetes manifests**

   ```bash
   kubectl apply -f k8s/namespace.yaml
   kubectl apply -f k8s/deployment.yaml
   kubectl apply -f k8s/service.yaml
   ```

8. **Wait for rollout**

   ```bash
   kubectl rollout status deployment/taskboard-backend -n taskboard
   kubectl get pods -n taskboard
   ```

9. **Smoke test `/health` endpoint**

   ```bash
   kubectl port-forward svc/taskboard-backend -n taskboard 5000:80 &
   curl -f http://localhost:5000/health
   ```

If any of these steps fail, the CD pipeline fails, ensuring that only healthy deployments are considered successful.

Permissions:

```yaml
permissions:
  contents: read
  packages: write
```

---

## Security & Quality

* **Static Analysis (SAST):**
  GitHub **CodeQL** code scanning is enabled for this repository via the Security tab. It analyzes the C++ code for common vulnerabilities and code issues.

* **CI Hardening:**

  * Minimal GitHub Actions `permissions` in both CI and CD workflows (principle of least privilege).
  * Unit tests integrated into CI (pipeline fails if tests fail).

* **Infrastructure as Code:**

  * Kubernetes manifests are stored in version control.
  * CI/CD workflows are defined as code in `.github/workflows/`.

---

## Possible Future Improvements

The current version focuses on the DevOps side. Possible extensions:

* Implement a real TaskBoard API (tasks, statuses, priorities).
* Add a database (e.g. PostgreSQL) and migrations (e.g. Flyway).
* Introduce container image scanning (e.g. Trivy).
* Add more advanced static analysis (clang-tidy, clang-format checks).
* Add Ingress and TLS termination for external HTTPS access in Kubernetes.
* Split the project into separate services (backend + frontend) and extend the pipeline accordingly.

For the purposes of the **Modern Practices in DevOps** course, the current implementation already demonstrates:

* End-to-end CI/CD,
* Docker-based delivery,
* Deployment to Kubernetes,
* Basic security and quality practices.
