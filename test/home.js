const addr = "https://0.0.0.0:8080"

document.addEventListener("DOMContentLoaded", function () {
    const serverList = document.querySelector(".server_list ul");

    fetch(addr + "/api/get_servers", {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        }
    })
        .then(response => response.json())
        .then(data => {
            data.servers.forEach(server => {
                const listItem = document.createElement("li");
                listItem.textContent = server;
                listItem.style.borderBottom = "solid black 1px";
                listItem.style.borderTop = "solid black 1px";
                listItem.addEventListener("click", function () {
                    const content = document.getElementById("content");
                    content.innerHTML = `
                        <div style="height: 100%; width: 100%; display: grid; grid-template-rows: 25% repeat(5, 15%);">
                            <h2 style="margin: 0; height: 100%; width: 100%; font-size: xxx-large; border-bottom: black solid 1px">Server address: ${data.address}</h2>
                            <div>
                                <div class="add-listener-button">
                                    <img id="add_endp" src="res/plus-circle.svg" />
                                    Add listener
                                </div>
                                <div class="listener-form" style="display: none;">
                                    <form id="listenerForm">
                                        <label for="port">Port:</label>
                                        <input type="text" id="port" name="port" required>
                                        <label for="method">Method:</label>
                                        <input type="text" id="method" name="method" required>
                                        <button type="submit">Save</button>
                                    </form>
                                </div>
                                ${data.ports
                        ? `<div>
                                    ${data.ports
                            .map(port => `<div style="border: 1px solid black;">Port: ${port}</div>`)
                            .join('')}
                                </div>`
                        : `<div>No listeners created yet.</div>`
                    }
                            </div>
                        </div>
                    `;

                    const addListenerButton = content.querySelector(".add-listener-button");
                    const listenerForm = content.querySelector(".listener-form");
                    const listenerFormSubmit = document.getElementById("listenerForm");

                    addListenerButton.addEventListener("click", function () {
                        listenerForm.style.display = "block";
                    });

                    listenerFormSubmit.addEventListener("submit", function (e) {
                        e.preventDefault(); // Prevent default form submission

                        // Collect form data
                        const port = document.getElementById("port").value;
                        const method = document.getElementById("method").value;

                        // Send a POST request to create a new listener
                        fetch(addr + "/api/create_listener", {
                            method: 'POST',
                            headers: {
                                'Content-Type': 'application/json',
                            },
                            body: JSON.stringify({
                                port: port,
                                method: method
                            })
                        })
                            .then(response => response.json())
                            .then(data => {
                                // Handle the response as needed
                                console.log(data);
                            })
                            .catch(error => {
                                console.error("Error creating a new listener:", error);
                            });
                    });
                });

                serverList.appendChild(listItem);
            });
        })
        .catch(error => {
            console.error("Error fetching server data:", error);
        });
});


document.getElementById("add").addEventListener("click", function () {
    const form = document.createElement("form");
    form.style = `
        background-color: lightgray;
        padding: 20px;
        border: 2px solid #ccc;
        border-radius: 10px;
        position:relative;
        top: 25%;
        height: 50%;
        width: 60%; /* Set the width to a smaller value */
        margin: 0 auto; /* Center horizontally */
    `;

    const serverIdInput = document.createElement("input");
    serverIdInput.type = "text";
    serverIdInput.placeholder = "Server ID";
    serverIdInput.name = "server_id";
    serverIdInput.style = `
        width: 100%;
        height: 10%;
        margin-bottom: 10px;
        border: 1px solid #ccc;
        border-radius: 5px;
        transition: border 0.3s;
    `;

    const serverAddressInput = document.createElement("input");
    serverAddressInput.type = "text";
    serverAddressInput.placeholder = "Server address";
    serverAddressInput.name = "server_address";
    serverAddressInput.style = `
        width: 100%;
        height: 10%;
        margin-bottom: 10px;
        border: 1px solid #ccc;
        border-radius: 5px;
        transition: border 0.3s;
    `;

    const submitButton = document.createElement("button");
    submitButton.type = "submit";
    submitButton.textContent = "Submit";
    submitButton.style = `
        width: 100%;
        padding: 10px;
        background-color: #007bff;
        color: #fff;
        border: none;
        border-radius: 5px;
        cursor: pointer;
    `;

    serverIdInput.addEventListener("focus", function () {
        serverIdInput.style.border = "1px solid #007bff";
    });

    serverIdInput.addEventListener("blur", function () {
        serverIdInput.style.border = "1px solid #ccc";
    });

    serverAddressInput.addEventListener("focus", function () {
        serverAddressInput.style.border = "1px solid #007bff";
    });

    serverAddressInput.addEventListener("blur", function () {
        serverAddressInput.style.border = "1px solid #ccc";
    });

    form.addEventListener("submit", function (event) {
        event.preventDefault();

        const serverId = serverIdInput.value;
        const serverAddress = serverAddressInput.value;

        const requestData = {
            id: serverId,
            address: serverAddress
        };

        fetch(addr + "/api/create_server", {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(requestData)
        })
            .then(response => response.json())
            .then(data => {
                console.log("Server created:", data);
            });

        console.log("Sending data:", requestData);

        serverIdInput.value = "";
        serverAddressInput.value = "";
    });

    form.appendChild(serverIdInput);
    form.appendChild(serverAddressInput);
    form.appendChild(submitButton);

    const content = document.getElementById("content");
    content.innerHTML = "";
    content.appendChild(form);
});