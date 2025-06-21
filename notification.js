export function logEvent(message) {
  const box = document.getElementById("infoBox");
  box.innerHTML = `📢 ${message}`;
  console.log("🪧", message);
}
