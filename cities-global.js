// cities-global.js

export const cityIcon = L.divIcon({
  className: "city-icon",
  html: "🏙️",
  iconSize: [10, 10]
});

export function addCitiesToMap(cities, map) {
  const markers = [];

  cities.forEach(city => {
    const marker = L.marker(city.latlng, { icon: cityIcon }).addTo(map);
    marker.bindTooltip(`${city.name} (${city.nation})`, { permanent: false });
    marker._icon.style.display = "none"; // hidden until zoomed in
    markers.push(marker);
  });

  map.on("zoomend", () => {
    const zoom = map.getZoom();
    markers.forEach(marker => {
      if (marker._icon) {
        marker._icon.style.display = zoom >= 5 ? "block" : "none";
      }
    });
  });
}

