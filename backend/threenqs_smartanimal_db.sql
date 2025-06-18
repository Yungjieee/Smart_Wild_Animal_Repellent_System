-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Jun 18, 2025 at 01:56 PM
-- Server version: 10.3.39-MariaDB-cll-lve
-- PHP Version: 8.1.32

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `threenqs_smartanimal_db`
--

-- --------------------------------------------------------

--
-- Table structure for table `sensor_data`
--

CREATE TABLE `sensor_data` (
  `id` int(11) NOT NULL,
  `temperature` float DEFAULT NULL,
  `humidity` float DEFAULT NULL,
  `distance` float DEFAULT NULL,
  `relay_status` tinyint(1) DEFAULT NULL,
  `vibration_status` tinyint(1) DEFAULT NULL,
  `motion_status` tinyint(1) DEFAULT NULL,
  `status_message` varchar(255) DEFAULT NULL,
  `mode_status` varchar(10) DEFAULT NULL,
  `timestamp` datetime DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;

--
-- Dumping data for table `sensor_data`
--

INSERT INTO `sensor_data` (`id`, `temperature`, `humidity`, `distance`, `relay_status`, `vibration_status`, `motion_status`, `status_message`, `mode_status`, `timestamp`) VALUES
(239, 32.1, 77, 179.3, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:47:53'),
(240, 32.3, 76, 178.02, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:48:07'),
(241, 32.6, 76, 5.07, 1, 0, 1, 'Danger: Animal approaching!', 'Auto', '2025-06-17 18:48:18'),
(242, 32.1, 77, 7.12, 0, 0, 0, 'Alert: Object close, no movement.', 'Auto', '2025-06-17 18:48:31'),
(243, 32.3, 77, 178.04, 1, 1, 1, 'Danger: Motion and vibration detected!', 'Auto', '2025-06-17 18:48:44'),
(244, 32.6, 77, 181.41, 0, 1, 0, 'Safe.', 'Auto', '2025-06-17 18:48:57'),
(245, 32.9, 77, 188.97, 1, 1, 0, 'Danger: Vibration detected!', 'Auto', '2025-06-17 18:49:07'),
(246, 32.5, 77, 10.15, 1, 1, 0, 'Safe.', 'Auto', '2025-06-17 18:49:20'),
(247, 32, 77, 178.41, 1, 1, 1, 'Danger: Motion and vibration detected!', 'Auto', '2025-06-17 18:49:40'),
(248, 32, 77, 180.1, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:49:53'),
(249, 32.3, 77, 178.84, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:50:04'),
(250, 32.2, 76, 190.23, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:50:15'),
(251, 32.1, 76, 179.23, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:50:28'),
(252, 32.5, 76, 180.57, 1, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:50:39'),
(253, 32.8, 77, 175.54, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:52:28'),
(254, 32.5, 77, 177.6, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:52:41'),
(255, 32.5, 77, 175.56, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:52:54'),
(256, 32.2, 77, 178.09, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:53:05'),
(257, 32.3, 77, 177.6, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:53:18'),
(258, 32.2, 77, 178.07, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:53:29'),
(259, 32.5, 77, 4.91, 1, 0, 1, 'Danger: Animal approaching!', 'Auto', '2025-06-17 18:53:43'),
(260, 32.9, 78, 5.83, 1, 1, 1, 'Danger: Animal confirmed!', 'Auto', '2025-06-17 18:53:55'),
(261, 32.5, 77, 9.11, 1, 1, 1, 'Danger: Animal confirmed!', 'Auto', '2025-06-17 18:54:05'),
(262, 32, 77, 175.58, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:54:18'),
(263, 32.4, 77, 178.48, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:54:29'),
(264, 32.5, 77, 177.26, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:54:39'),
(265, 32.2, 77, 181.9, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:54:51'),
(266, 32.5, 77, 178.93, 1, 0, 1, 'Danger: Animal approaching!', 'Auto', '2025-06-17 18:55:04'),
(267, 32.9, 77, 178.89, 1, 0, 1, 'Danger: Animal approaching!', 'Auto', '2025-06-17 18:55:15'),
(268, 32.2, 76, 180.98, 1, 0, 1, 'Danger: Animal approaching!', 'Auto', '2025-06-17 18:55:25'),
(269, 32.7, 77, 179.3, 1, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:55:36'),
(270, 32.4, 76, 177.6, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:55:47'),
(271, 32, 77, 178.93, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:55:57'),
(272, 32.1, 77, 175.56, 1, 0, 0, 'Safe: No activity.', 'Manual', '2025-06-17 18:56:10'),
(273, 32.1, 77, 189.41, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:56:22'),
(274, 32, 77, 189.82, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:56:35'),
(275, 32.5, 77, 177.6, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:56:48'),
(276, 32.2, 77, 179.74, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:56:59'),
(277, 32.7, 77, 177.6, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:57:09'),
(278, 32.5, 77, 178.11, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:57:20'),
(279, 32.8, 77, 180.17, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:57:30'),
(280, 32, 77, 180.18, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:57:40'),
(281, 32.6, 77, 191.52, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:57:53'),
(282, 32.6, 77, 178.43, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:58:05'),
(283, 32.9, 77, 175.51, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:58:15'),
(284, 32.7, 77, 178.47, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:58:26'),
(285, 32.1, 77, 176.39, 0, 0, 0, 'Safe: No activity.', 'Auto', '2025-06-17 18:58:39'),
(286, 32.2, 77, 179.35, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:58:51'),
(287, 32.4, 77, 191.95, 0, 0, 1, 'Alert: Motion detected, but not near.', 'Auto', '2025-06-17 18:59:02');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `sensor_data`
--
ALTER TABLE `sensor_data`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `sensor_data`
--
ALTER TABLE `sensor_data`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=288;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
