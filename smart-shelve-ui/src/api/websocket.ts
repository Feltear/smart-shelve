import { updateTable } from "./../components/update_table";

const configData = {
  shelf: null,
  products: [],
};

const liveData: Record<string, number> = {};

export class WebSocketClient {
  private socket: WebSocket;

  constructor(private url: string) {
    this.socket = new WebSocket(this.url);

    this.socket.onopen = () => {
      console.log(`[WebSocket] Connecté à ${this.url}`);
      this.socket.send("load_config");
    };

    this.socket.onmessage = (event) => {
      console.log(`[WebSocket] Message reçu :`, event.data);
      this.onMessage(event.data);
    };

    this.socket.onclose = () => {
      console.log(`[WebSocket] Déconnecté de ${this.url}`);
    };

    this.socket.onerror = (error) => {
      console.error(`[WebSocket] Erreur :`, error);
    };

    document
      .getElementById("reload-config-btn")
      ?.addEventListener("click", () => {
        if (this.socket.readyState === WebSocket.OPEN) {
          this.socket.send("load_config");
          console.log("🔄 Demande de rechargement de configuration envoyée.");
        } else {
          console.warn("⚠️ WebSocket non connecté.");
        }
      });

    document.getElementById("save-btn")?.addEventListener("click", () => {
      if (this.socket.readyState === WebSocket.OPEN) {
        this.socket.send("calibrate_shelves_height");
        console.log("💾 Demande d’écriture envoyée");
      }
    });
  }

  send(data: string): void {
    if (this.socket.readyState === WebSocket.OPEN) {
      this.socket.send(data);
    } else {
      console.warn("[WebSocket] Socket non ouvert");
    }
  }

  onMessage(data: any): void {
    console.log("[WebSocket] Message (override)", data);
    const parsedData = JSON.parse(data);

    if (parsedData.type == "config_data") {
      configData.shelf = parsedData.payload.shelf;
      configData.products = parsedData.payload.products;
      console.log("📦 Configuration reçue :", configData);
      updateDefinitions(configData);
    } else {
      for (const key in parsedData) {
        const value = parsedData[key];
        liveData[key] = value;

        switch (key) {
          case "product_1_quantity":
            updateTable(parsedData);
            break;
          case "product_2_quantity":
            updateTable(parsedData);
            break;
          case "product_3_quantity":
            updateTable(parsedData);
            break;
          default:
            console.warn("Clé inconnue reçue :", key, value);
        }
      }
    }
  }
}

function updateDefinitions(config: typeof configData) {
  if (config.shelf) {
    const { width, height, depth } = config.shelf;
    document.getElementById("shelf-width")!.textContent = width;
    document.getElementById("shelf-height")!.textContent = height;
    document.getElementById("shelf-depth")!.textContent = depth;
  }
  const tbody = document.getElementById("products-body")!;
  tbody.innerHTML = ""; // Nettoyer d’abord

  config.products.forEach((product, index) => {
    const tr = document.createElement("tr");
    const rowId = `product_${index + 1}_quantity`;
    tr.id = rowId;
    const { name, width, height, thickness } = product;
    tr.innerHTML = `
      <td>${name}</td>
      <td>${width}</td>
      <td>${height}</td>
      <td>${thickness}</td>
      <td class="quantity">${liveData[rowId] ?? "Inconnue"}</td>
    `;

    tbody.appendChild(tr);
  });
}
