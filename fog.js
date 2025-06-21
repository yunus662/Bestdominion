import * as L from "https://unpkg.com/leaflet@1.9.4/dist/leaflet-src.esm.js";

export function createFogLayer(map) {
  const canvas = document.createElement("canvas");
  canvas.width = map.getSize().x;
  canvas.height = map.getSize().y;
  canvas.style.position = "absolute";
  canvas.style.top = "0";
  canvas.style.left = "0";
  canvas.style.pointerEvents = "none";
  canvas.style.zIndex = "500"; // above tiles, below markers

  const fogPane = map.getPanes().overlayPane;
  fogPane.appendChild(canvas);
  const ctx = canvas.getContext("2d");

  let revealedPoints = [];

  function drawFog() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = "rgba(0, 0, 0, 0.6)";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ctx.globalCompositeOperation = "destination-out";
    revealedPoints.forEach(({ latlng, radius }) => {
      const point = map.latLngToContainerPoint(latlng);
      const r = radius || 80;
      const gradient = ctx.createRadialGradient(point.x, point.y, 0, point.x, point.y, r);
      gradient.addColorStop(0, "rgba(0,0,0,0.8)");
      gradient.addColorStop(1, "rgba(0,0,0,0)");
      ctx.fillStyle = gradient;
      ctx.beginPath();
      ctx.arc(point.x, point.y, r, 0, Math.PI * 2);
      ctx.fill();
    });
    ctx.globalCompositeOperation = "source-over";
  }

  function reveal(latlng, radius = 100) {
    revealedPoints.push({ latlng, radius });
    drawFog();
  }

  function resetFog() {
    revealedPoints = [];
    drawFog();
  }

  map.on("move zoom resize", () => {
    canvas.width = map.getSize().x;
    canvas.height = map.getSize().y;
    drawFog();
  });

  drawFog();

  return { reveal, resetFog };
}


