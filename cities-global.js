// cities-global.js
// Production-grade city icon renderer for the Conqueror Engine Map

import L from "leaflet";

/**
 * @constant {L.DivIcon} cityIcon
 * A Leaflet div icon used to represent cities on the map.
 */
export const cityIcon = L.divIcon({
  className: "city-icon",
  html: "🏙️",
  iconSize: [10, 10]
});

/**
 * @function addCitiesToMap
 * Adds a set of city markers to the Leaflet map and manages their visibility based on zoom level.
 *
 * @param {Array<Object>} cities - Array of city objects, each with `latlng`, `name`, and `nation`.
 * @param {L.Map} map - Leaflet map instance to which cities will be added.
 */
export function addCitiesToMap(cities, map) {
  const markers = [];

  cities.forEach(city => {
    const marker = L.marker(city.latlng, { icon: cityIcon }).addTo(map);
    marker.bindTooltip(`${city.name} (${city.nation})`, { permanent: false });
    marker._icon.style.display = "none"; // Initially hidden until appropriate zoom level
    markers.push(marker);
  });

  map.on("zoomend", () => {
    const zoom = map.getZoom();
    const visible = zoom >= 5;
    markers.forEach(marker => {
      if (marker._icon) {
        marker._icon.style.display = visible ? "block" : "none";
      }
    });
  });
}
