<?php
error_reporting(0);
include_once("dbconnect.php");

$temperature = $_GET['temperature'];
$humidity = $_GET['humidity'];
$distance = $_GET['distance'];
$relay = $_GET['relay'];
$vibration = $_GET['vibration'];
$motion = $_GET['motion'];
$status = $_GET['status'];
$mode = $_GET['mode'];

// Check for essential required fields (prevent null row)
if (
    !isset($temperature) || $temperature === "" || $temperature == 0 ||
    !isset($humidity) || $humidity === "" || $humidity == 0 ||
    !isset($distance) || $distance === "" ||
    !isset($status) || $status === "" ||
    !isset($mode) || $mode === ""
) {
    echo "Missing required data. Skipping insert.";
    exit();
}

$sql = "INSERT INTO sensor_data (temperature, humidity, distance, relay_status, vibration_status, motion_status, status_message, mode_status)
VALUES ('$temperature', '$humidity', '$distance', '$relay', '$vibration', '$motion', '$status', '$mode')";

if ($conn->query($sql) === TRUE) {
    echo "Data inserted successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

?>
