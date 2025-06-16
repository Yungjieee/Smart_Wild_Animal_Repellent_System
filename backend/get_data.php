<?php
header('Content-Type: application/json');
include_once("dbconnect.php");

$range = $_GET['range'] ?? 'daily';

switch ($range) {
  case 'weekly':
    $sql = "SELECT timestamp, temperature, humidity, relay_status 
            FROM sensor_data 
            WHERE timestamp >= NOW() - INTERVAL 7 DAY 
            ORDER BY timestamp ASC";
    break;
  case 'monthly':
    $sql = "SELECT timestamp, temperature, humidity, relay_status 
            FROM sensor_data 
            WHERE timestamp >= NOW() - INTERVAL 1 MONTH 
            ORDER BY timestamp ASC";
    break;
  default:
    $sql = "SELECT timestamp, temperature, humidity, relay_status 
            FROM sensor_data 
            WHERE timestamp >= NOW() - INTERVAL 1 DAY 
            ORDER BY timestamp ASC";
    break;
}

$result = $conn->query($sql);

$data = [];

if ($result && $result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $data[] = [
            "timestamp" => $row["timestamp"],
            "temperature" => floatval($row["temperature"]),
            "humidity" => floatval($row["humidity"]),
            "relay_status" => intval($row["relay_status"])
        ];
    }
    // Optional: reverse to make oldest first
    $data = array_reverse($data);
}

echo json_encode($data);
?>
