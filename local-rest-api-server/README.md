# Local REST API Server

This project is a simple REST API server that handles file uploads and deletions using Node.js and Express. It is designed for testing purposes and provides endpoints for managing files.

## Project Structure

```
local-rest-api-server
├── src
│   ├── server.js
│   ├── routes
│   │   └── fileRoutes.js
│   ├── controllers
│   │   └── fileController.js
│   └── middleware
│       └── errorHandler.js
├── package.json
├── .env
└── README.md
```

## Getting Started

### Prerequisites

- Node.js (version 14 or higher)
- npm (Node package manager)

### Installation

1. Clone the repository:

   ```
   git clone <repository-url>
   cd local-rest-api-server
   ```

2. Install the dependencies:

   ```
   npm install
   ```

3. Create a `.env` file in the root directory and specify the following environment variables:

   ```
   PORT=3000
   ```

### Running the Server

To start the server, run the following command:

```
npm start
```

The server will be running on `http://localhost:3000`.

### API Endpoints

- **Upload a File**
  - **Endpoint:** `POST /upload`
  - **Description:** Uploads a file to the server.
  - **Request Body:** Form-data with the file included.

- **Delete a File**
  - **Endpoint:** `DELETE /file/:filename`
  - **Description:** Deletes a specified file from the server.
  - **URL Parameter:** `filename` - The name of the file to delete.

### Error Handling

The application includes middleware for error handling. Any errors encountered during file operations will be captured and an appropriate response will be sent to the client.

### License

This project is licensed under the MIT License.