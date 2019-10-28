-- phpMyAdmin SQL Dump
-- version 4.6.6deb4
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Mar 13, 2019 at 07:06 PM
-- Server version: 10.1.37-MariaDB-0+deb9u1
-- PHP Version: 7.0.33-0+deb9u3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: "EMCA_DB"
--

-- --------------------------------------------------------

--
-- Table structure for table "GAS_INFO"
--

CREATE TABLE GAS_INFO (
  ID int(11) NOT NULL,
  TIME timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  GAS_TYPE varchar(10) NOT NULL,
  TEMP float NOT NULL,
  HUM float NOT NULL,
  PRE float NOT NULL,
  WE float NOT NULL,
  AE float NOT NULL,
  PPB float NOT NULL,
  PPB_C float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Indexes for dumped tables
--

--
-- Indexes for table GAS_INFO
--
ALTER TABLE GAS_INFO
  ADD PRIMARY KEY (ID);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table GAS_INFO
--
ALTER TABLE GAS_INFO
  MODIFY ID int(11) NOT NULL AUTO_INCREMENT;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
