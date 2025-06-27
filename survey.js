export function discoverResources(region) {
  const findings = [];
  if (Math.random() > 0.8) findings.push("Oil");
  if (Math.random() > 0.85) findings.push("Uranium");
  if (Math.random() > 0.9) findings.push("Rare Earths");
  return findings;
}
