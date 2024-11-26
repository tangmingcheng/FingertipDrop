// 全局 WebSocket 变量
let webSocket;
let wsAddress;
let fileMetadataSent = false;  // 标记元数据是否已发送


document.addEventListener("DOMContentLoaded", () => {
    const dropZone = document.getElementById("dropZone");

    if (!dropZone) {
        console.error("Drop zone element not found!");
        return;
    }

    dropZone.addEventListener("dragover", (event) => {
        event.preventDefault();
        dropZone.classList.add("dragover");
    });

    dropZone.addEventListener("dragleave", () => {
        dropZone.classList.remove("dragover");
    });

    dropZone.addEventListener("drop", (event) => {
        event.preventDefault();
        dropZone.classList.remove("dragover");

        const files = event.dataTransfer.files;
        if (files.length > 0) {
            const file = files[0];
            console.log("File dropped:", file.name);

            const fileInput = document.getElementById("fileInput");
            const dataTransfer = new DataTransfer();
            dataTransfer.items.add(file);
            fileInput.files = dataTransfer.files;

            //sendFile();
        }
    });
});


// 等待页面加载完成再执行
window.onload = function() {
    console.log("Binding events");  // 添加调试信息

    // 获取 WebSocket 地址和端口
    const wsConfigElement = document.getElementById("wsConfig");
    const address = wsConfigElement.getAttribute("data-address");
    const port = wsConfigElement.getAttribute("data-port");
    wsAddress = "ws://" + address + ":" + port;

    console.log("WebSocket address is:", wsAddress);

    startConnection();
};

// 定义 WebSocket 连接函数
function startConnection() {
    console.log("startConnection function called");

    // 检查是否已经存在 WebSocket 连接
    if (webSocket && webSocket.readyState === WebSocket.OPEN) {
        console.log("WebSocket is already connected.");
        return;
    }

    webSocket = new WebSocket(wsAddress);
    webSocket.binaryType = "arraybuffer";  // 设置为二进制传输

    webSocket.onopen = function() {
        console.log("WebSocket connected");
        const deviceInfo = navigator.userAgent;
        webSocket.send(JSON.stringify({ type: "deviceInfo", info: deviceInfo }));
        console.log("Device info sent:", deviceInfo);
    };

    webSocket.onmessage = function(event) {
        const message = event.data;

        // 检查服务器是否发来确认接收元数据的消息
        if (message === "READY_TO_RECEIVE_FILE") {
            console.log("Server is ready to receive file data");
            if (fileMetadataSent) {
                sendBinaryFile();
            }
        } else {
            console.log("Message received:", message);
            alert("Received clipboard content: " + message);
        } 
        
    };

    webSocket.onerror = function(error) {
        console.error("WebSocket error:", error);
    };

    webSocket.onclose = function() {
        console.log("WebSocket connection closed");
        // 重置 webSocket 变量，便于重新连接
        webSocket = null;
        fileMetadataSent = false;  // 重置元数据标记
    };
}

// 定义发送文件函数
function sendFile() {
    const fileInput = document.getElementById("fileInput");
    const file = fileInput.files[0];

    if (!file) {
        console.error("No file selected");
        alert("Please select a file first");
        return;
    }

    if (webSocket && webSocket.readyState === WebSocket.OPEN) {
        // 构建文件元数据
        const metadata = {
            type: "fileMetadata",
            filename: file.name,
            filetype: file.type,
            size: file.size
        };
        webSocket.send(JSON.stringify(metadata));
        console.log("Metadata sent:", metadata);

        // 设置标记，表示元数据已发送
        fileMetadataSent = true;

    } else {
        console.warn("WebSocket is not connected.");
        alert("Please connect to the WebSocket server first.");
    }
}

// 发送文件的二进制数据
function sendBinaryFile() {
    const fileInput = document.getElementById("fileInput");
    const file = fileInput.files[0];

    const reader = new FileReader();
    reader.onload = function(event) {
        const arrayBuffer = event.target.result;
        webSocket.send(arrayBuffer);  // 发送文件的二进制数据
        console.log("File data sent:", file.name);
    };
    reader.readAsArrayBuffer(file);
}


// 发送图片文件
function sendImage() {
    const fileInput = document.getElementById("fileInput");
    const file = fileInput.files[0];

    if (!file) {
        console.error("No file selected");
        alert("Please select an image file first");
        return;
    }

    if (webSocket && webSocket.readyState === WebSocket.OPEN) {
        const reader = new FileReader();
        reader.onload = function(event) {
            const arrayBuffer = event.target.result;
            webSocket.send(arrayBuffer);  // 通过 WebSocket 发送二进制数据
            console.log("Image sent:", file.name);
        };
        reader.readAsArrayBuffer(file);  // 将文件读取为 ArrayBuffer
    } else {
        console.warn("WebSocket is not connected.");
        alert("Please connect to the WebSocket server first.");
    }
}
