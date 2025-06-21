export const buildings = [];

export function addBuilding(type, latlng, iconUrl) {
  const icon = L.icon({
    iconUrl,
    iconSize: [28, 28],
    className: `building-icon ${type}`
  });

  const marker = L.marker(latlng, { icon }).addTo(map);
  marker.buildingType = type;
  buildings.push(marker);
  return marker;
}
