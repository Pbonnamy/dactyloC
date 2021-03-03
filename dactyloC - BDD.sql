-- phpMyAdmin SQL Dump
-- version 4.9.5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Jan 06, 2021 at 03:29 PM
-- Server version: 5.7.24
-- PHP Version: 7.4.1

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `cproject`
--

-- --------------------------------------------------------

--
-- Table structure for table `comettre`
--

CREATE TABLE `comettre` (
  `idUser` int(10) UNSIGNED NOT NULL,
  `idErreur` int(10) UNSIGNED NOT NULL,
  `gravite` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `completer`
--

CREATE TABLE `completer` (
  `idUser` int(10) UNSIGNED NOT NULL,
  `idCours` int(10) UNSIGNED NOT NULL,
  `terminer` tinyint(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `contient`
--

CREATE TABLE `contient` (
  `idCours` int(10) UNSIGNED NOT NULL,
  `idImage` int(10) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `contient`
--

INSERT INTO `contient` (`idCours`, `idImage`) VALUES
(1, 1),
(2, 2),
(3, 3),
(4, 4),
(5, 5),
(6, 6),
(7, 7);

-- --------------------------------------------------------

--
-- Table structure for table `cours`
--

CREATE TABLE `cours` (
  `id` int(10) UNSIGNED NOT NULL,
  `description` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `cours`
--

INSERT INTO `cours` (`id`, `description`) VALUES
(1, 'nombres'),
(2, 'speciale'),
(3, 'auriculaire'),
(4, 'annulaire'),
(5, 'majeur'),
(6, 'index'),
(7, 'accent');

-- --------------------------------------------------------

--
-- Table structure for table `erreur`
--

CREATE TABLE `erreur` (
  `id` int(10) UNSIGNED NOT NULL,
  `contenu` varchar(20) DEFAULT NULL,
  `idCours` int(10) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `erreur`
--

INSERT INTO `erreur` (`id`, `contenu`, `idCours`) VALUES
(1, 'number', 1),
(2, 'speciale', 2),
(3, 'auriculaire', 3),
(4, 'annulaire', 4),
(5, 'majeur', 5),
(6, 'index', 6),
(7, 'accent', 7);

-- --------------------------------------------------------

--
-- Table structure for table `image`
--

CREATE TABLE `image` (
  `id` int(10) UNSIGNED NOT NULL,
  `lien` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `image`
--

INSERT INTO `image` (`id`, `lien`) VALUES
(1, 'asset/cours/cours1.jpg'),
(2, 'asset/cours/cours2.jpg'),
(3, 'asset/cours/cours3.jpg'),
(4, 'asset/cours/cours4.jpg'),
(5, 'asset/cours/cours5.jpg'),
(6, 'asset/cours/cours6.jpg'),
(7, 'asset/cours/cours7.jpg');

-- --------------------------------------------------------

--
-- Table structure for table `score`
--

CREATE TABLE `score` (
  `id` int(10) UNSIGNED NOT NULL,
  `valeur` int(11) NOT NULL,
  `date` date NOT NULL,
  `idUser` int(10) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `user`
--

CREATE TABLE `user` (
  `id` int(10) UNSIGNED NOT NULL,
  `username` varchar(10) NOT NULL,
  `password` varchar(10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `comettre`
--
ALTER TABLE `comettre`
  ADD KEY `idUser` (`idUser`),
  ADD KEY `idErreur` (`idErreur`);

--
-- Indexes for table `completer`
--
ALTER TABLE `completer`
  ADD KEY `idUser` (`idUser`),
  ADD KEY `idCours` (`idCours`);

--
-- Indexes for table `contient`
--
ALTER TABLE `contient`
  ADD KEY `idCours` (`idCours`),
  ADD KEY `idImage` (`idImage`);

--
-- Indexes for table `cours`
--
ALTER TABLE `cours`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `erreur`
--
ALTER TABLE `erreur`
  ADD PRIMARY KEY (`id`),
  ADD KEY `idCours` (`idCours`);

--
-- Indexes for table `image`
--
ALTER TABLE `image`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `score`
--
ALTER TABLE `score`
  ADD PRIMARY KEY (`id`),
  ADD KEY `idUser` (`idUser`);

--
-- Indexes for table `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `cours`
--
ALTER TABLE `cours`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;

--
-- AUTO_INCREMENT for table `erreur`
--
ALTER TABLE `erreur`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;

--
-- AUTO_INCREMENT for table `image`
--
ALTER TABLE `image`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;

--
-- AUTO_INCREMENT for table `score`
--
ALTER TABLE `score`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `user`
--
ALTER TABLE `user`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `comettre`
--
ALTER TABLE `comettre`
  ADD CONSTRAINT `comettre_ibfk_1` FOREIGN KEY (`idUser`) REFERENCES `user` (`id`),
  ADD CONSTRAINT `comettre_ibfk_2` FOREIGN KEY (`idErreur`) REFERENCES `erreur` (`id`);

--
-- Constraints for table `completer`
--
ALTER TABLE `completer`
  ADD CONSTRAINT `completer_ibfk_1` FOREIGN KEY (`idUser`) REFERENCES `user` (`id`),
  ADD CONSTRAINT `completer_ibfk_2` FOREIGN KEY (`idCours`) REFERENCES `cours` (`id`);

--
-- Constraints for table `contient`
--
ALTER TABLE `contient`
  ADD CONSTRAINT `contient_ibfk_1` FOREIGN KEY (`idCours`) REFERENCES `cours` (`id`),
  ADD CONSTRAINT `contient_ibfk_2` FOREIGN KEY (`idImage`) REFERENCES `image` (`id`);

--
-- Constraints for table `erreur`
--
ALTER TABLE `erreur`
  ADD CONSTRAINT `erreur_ibfk_1` FOREIGN KEY (`idCours`) REFERENCES `cours` (`id`);

--
-- Constraints for table `score`
--
ALTER TABLE `score`
  ADD CONSTRAINT `score_ibfk_1` FOREIGN KEY (`idUser`) REFERENCES `user` (`id`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
