const messageInput = document.getElementById('message-input');
const sendButton = document.getElementById('send-button');
const messagesDiv = document.getElementById('messages');

const socket = new WebSocket('ws://localhost:9000'); // Sambungkan ke server WebSocket

socket.onmessage = function(event) {
    const newMessage = document.createElement('div');
    newMessage.textContent = event.data;
    messagesDiv.appendChild(newMessage);
};

sendButton.addEventListener('click', function() {
    const message = messageInput.value.trim();
    if (message) {
        socket.send(message);
        messageInput.value = '';
    }
});
