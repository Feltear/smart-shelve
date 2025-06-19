export function updateTable(data: any) {
  for (const key in data) {
    const value = data[key];
    const row = document.getElementById(key);
    if (row) {
      const quantityCell = row.querySelector(".quantity");
      if (quantityCell) {
        quantityCell.textContent = value.toString();
      }
    }
  }
}
