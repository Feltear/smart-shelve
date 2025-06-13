import "./style.css";

import { createHorizontalTable } from "./components/table";

//Test
const headers = ["Étagère", "Produits", "Quantité"];
const data = [["1", "33x22x21", "2"]];

document.addEventListener("DOMContentLoaded", () => {
  const table = createHorizontalTable(headers, data);
  document.body.appendChild(table);
});
