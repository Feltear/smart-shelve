export function createHorizontalTable(
  headers: string[],
  rows: string[][]
): HTMLTableElement {
  const table = document.createElement("table");

  const thead = document.createElement("thead");
  const headerRow = document.createElement("tr");

  for (const header of headers) {
    const th = document.createElement("th");
    th.textContent = header;
    headerRow.appendChild(th);
  }
  thead.appendChild(headerRow);
  table.appendChild(thead);

  const tbody = document.createElement("tbody");

  for (const rowData of rows) {
    const tr = document.createElement("tr");
    for (const cellData of rowData) {
      const td = document.createElement("td");
      td.textContent = cellData;
      tr.appendChild(td);
    }
    tbody.appendChild(tr);
  }
  table.appendChild(tbody);

  return table;
}
