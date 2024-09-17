### **Stage 1: Initial Setup and Research (Day 1–3)**
- **WP1.1: Research and Planning (Team effort)**
  - **Deliverables**: Understanding HTTP/1.1 specification, learning about socket programming, and reviewing basic web server architectures. The team should align on the architecture (use of select, poll, epoll, threading).
  - **Duration**: 2–3 days
  - **Output**: Documentation summarizing key HTTP features, C++ classes structure, and technology decisions.

---

### **Stage 2: Core Infrastructure (Day 4–10)**

#### **WP2.1: HTTP Request Parsing and Validation (Coder A)**
- **Deliverables**: A module that can receive HTTP requests and parse the request line, headers, and body according to the HTTP/1.1 specification.
  - **Details**:
    - Parsing different HTTP methods (GET, POST, PUT, DELETE).
    - Handling headers like `Host`, `Content-Length`, and `Transfer-Encoding`.
  - **Duration**: 7 days
  - **Dependencies**: None
  - **Collaboration points**: Integrate with Coder B's routing module for request handling.

#### **WP2.2: Connection Handling (Coder B)**
- **Deliverables**: Create a networking module to manage multiple clients and handle socket connections.
  - **Details**:
    - Non-blocking I/O with `select`, `poll`, or `epoll`.
    - Handle multiple clients at once.
    - Support persistent connections (keep-alive).
  - **Duration**: 7 days
  - **Dependencies**: None
  - **Collaboration points**: Synchronize with WP2.1 to test parsing from incoming connections.

#### **WP2.3: Response Formatting and Status Codes (Lauri)**
- **Deliverables**: Create a module that formats and sends HTTP responses back to the client.
  - **Details**:
    - Support different status codes (200, 404, 500, etc.).
    - Format HTTP response headers and body.
    - Handle static file serving (reading from disk).
  - **Duration**: 7 days
  - **Dependencies**: None
  - **Collaboration points**: Integrate with WP2.2 to handle responses for incoming requests.

---

### **Stage 3: Core Functionality Implementation (Day 11–18)**

#### **WP3.1: Routing and Request Handling (Coder B)**
- **Deliverables**: Implement routing logic to map incoming requests to appropriate resources.
  - **Details**:
    - Create basic routing for static files.
    - Handle directory traversal.
    - Return 404 responses for non-existent resources.
  - **Duration**: 7 days
  - **Dependencies**: Requires parsed requests from WP2.1.
  - **Collaboration points**: Work with Coder A to process the parsed requests.

#### **WP3.2: CGI Implementation (Tomi)**
- **Deliverables**: Implement Common Gateway Interface (CGI) to support dynamic content.
  - **Details**:
    - Create a module to execute external programs/scripts (e.g., Python, PHP).
    - Pass HTTP environment variables to CGI scripts and return the output.
    - Ensure it works for POST and GET methods.
  - **Duration**: 7 days
  - **Dependencies**: Requires WP3.1 routing and connection handling to be functional.
  - **Collaboration points**: Test dynamic routes in conjunction with Coder B’s routing module.

#### **WP3.3: Configuration File Parsing (Aaro)**
- **Deliverables**: Implement a configuration system to define virtual hosts, ports, and routes.
  - **Details**:
    - Parse and validate a configuration file (e.g., specifying server blocks, routes, ports).
    - Allow flexibility in configuring server behavior.
  - **Duration**: 7 days
  - **Dependencies**: None, but it needs to be integrated with WP3.1 for routing.
  - **Collaboration points**: Coder B to ensure the configuration supports the required routes.

---

### **Stage 4: Advanced Features and Enhancements (Day 19–25)**

#### **WP4.1: Chunked Transfer Encoding (Tomi)**
- **Deliverables**: Add support for chunked transfer encoding for large payloads.
  - **Duration**: 5–7 days
  - **Dependencies**: Requires WP2.1 to support reading large requests.
  - **Collaboration points**: Ensure it works with WP2.2 for proper response handling.

#### **WP4.2: Error Handling and Logging (Coder C)**
- **Deliverables**: Implement error handling and logging for debugging and monitoring.
  - **Details**:
    - Handle timeouts, bad requests, and malformed headers.
    - Log request/response details and server errors.
  - **Duration**: 5 days
  - **Dependencies**: Can be implemented independently, but needs input from all coders for error scenarios.
  - **Collaboration points**: Coordinate with other team members for proper logging of errors from all components.

#### **WP4.3: Multi-Threading or Process Handling for Concurrent Requests (Coder B)**
- **Deliverables**: Implement a system for handling concurrent requests using multi-threading or multi-processing.
  - **Duration**: 5–7 days
  - **Dependencies**: Requires solid connection handling and routing from previous WPs.
  - **Collaboration points**: Ensure that other components can handle concurrent requests.

---

### **Stage 5: Testing and Final Adjustments (Day 26–30)**

#### **WP5.1: Unit and Integration Testing (Team effort)**
- **Deliverables**: Write tests for each module and perform integration testing.
  - **Details**:
    - Unit test for request parsing, connection handling, and response formatting.
    - End-to-end tests simulating real HTTP requests.
  - **Duration**: 5 days
  - **Collaboration points**: The entire team will need to work together to debug and test integration.

#### **WP5.2: Documentation and Code Cleanup (Team effort)**
- **Deliverables**: Complete documentation and clean up code for submission.
  - **Duration**: 2 days
  - **Output**: Final project submission, with well-documented code, configuration files, and user instructions.

---

### **Milestones & Dependencies Overview**

1. **Milestone 1** (Day 3): Research complete, architecture plan agreed upon.
2. **Milestone 2** (Day 10): Core infrastructure in place (parsing, connection handling, basic responses).
3. **Milestone 3** (Day 18): Routing, CGI, and configuration file handling complete.
4. **Milestone 4** (Day 25): Advanced features (chunked encoding, concurrency) are functional.
5. **Milestone 5** (Day 30): Testing and documentation complete, project finalized.

---

### **Summary of Work Packages:**
- **Coder A**: Focus on request parsing, CGI, and chunked transfer encoding.
- **Coder B**: Focus on connection handling, routing, and concurrency.
- **Coder C**: Focus on response formatting, configuration file handling, error handling, and logging.

By following this plan, the team can work in parallel on distinct components while synchronizing at key points to ensure the overall system integrates smoothly.
