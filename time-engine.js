// time-engine.js
export const GAME_SPEED = 60; // 1 real second = 1 in-game minute

export function startGameClock(onTick) {
  let gameMinutes = 0;
  setInterval(() => {
    gameMinutes += 1;
    onTick(gameMinutes);
  }, 1000 / (GAME_SPEED / 60));
}

export function moveUnitOverTime(unit, fromLatLng, toLatLng, durationMs, onUpdate, onComplete) {
  const start = performance.now();
  const deltaLat = toLatLng.lat - fromLatLng.lat;
  const deltaLng = toLatLng.lng - fromLatLng.lng;

  function animate(now) {
    const elapsed = now - start;
    const t = Math.min(elapsed / durationMs, 1);
    const newLat = fromLatLng.lat + deltaLat * t;
    const newLng = fromLatLng.lng + deltaLng * t;
    unit.setLatLng([newLat, newLng]);
    onUpdate?.(t);

    if (t < 1) requestAnimationFrame(animate);
    else onComplete?.();
  }

  requestAnimationFrame(animate);
}
