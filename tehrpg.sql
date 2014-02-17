DROP TABLE IF EXISTS `Characters`;
DROP TABLE IF EXISTS `Walls`;
DROP TABLE IF EXISTS `Rooms`;
DROP TABLE IF EXISTS `Users`;

CREATE TABLE `Users` (
	`id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
	`username` VARCHAR(255) UNIQUE NOT NULL,
	`hashedpasswd` BINARY(64) NOT NULL,
	`permissions` SMALLINT UNSIGNED NOT NULL,
	INDEX (`username`)
) ENGINE=InnoDB;

CREATE TABLE `Rooms` (
	`id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
	`xpos` BIGINT NOT NULL,
	`ypos` BIGINT NOT NULL,
	`zpos` TINYINT NOT NULL,
	`description` TEXT NOT NULL,
	UNIQUE KEY `position` (`xpos`, `ypos`, `zpos`),
	INDEX (`xpos`),
	INDEX (`ypos`),
	INDEX (`zpos`)
) ENGINE=InnoDB;

CREATE TABLE `Walls` (
	`id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
	`side` ENUM('up','north','east') NOT NULL,
	`durability` INT,
	`material` VARCHAR(255),
	`room_id` INT UNSIGNED,
	FOREIGN KEY (`room_id`) REFERENCES `Rooms` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB;

CREATE TABLE `Characters` (
	`id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
	`name` VARCHAR(255) UNIQUE NOT NULL,
	`strength` TINYINT UNSIGNED NOT NULL,
	`constitution` TINYINT UNSIGNED NOT NULL,
	`dexterity` TINYINT UNSIGNED NOT NULL,
	`intelligence` TINYINT UNSIGNED NOT NULL,
	`wisdom` TINYINT UNSIGNED NOT NULL,
	`charisma` TINYINT UNSIGNED NOT NULL,
	`user_id` INT UNSIGNED,
	FOREIGN KEY (`user_id`) REFERENCES `Users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
	`room_id` INT UNSIGNED,
	FOREIGN KEY (`room_id`) REFERENCES `Rooms` (`id`) ON UPDATE CASCADE,
	INDEX (`name`)
) ENGINE=InnoDB;

INSERT INTO `Users` VALUES (NULL, "root", 0x0, 65535);
INSERT INTO `Users` VALUES (NULL, "dasterin", 0x7a3eff4c26e221298cab1d5dfb39ccd7ca8c199291e40c327feae06f91c3bb36e10a618328bf88d91e283bd3d1fe4acc14c2e50ae4b1c51ed754579c6a302173, 7);
INSERT INTO `Users` VALUES (NULL, "t", 0xf1abcfb47c903b9c3aa787dff53347cc0adafea46d22b70c84169d5e87e78f3f6c7b41fd26ffaa078ae0715c6a5be1904aad7f7c6dd4deaf1e67d80dc92c9a1d, 7);
INSERT INTO `Rooms` VALUES (NULL, 0,0,0, "A basic and empty room.");
INSERT INTO `Characters` VALUES (NULL, "Dasterin", 10, 10, 10, 10, 10, 10, (SELECT `id` FROM `Users` WHERE `username` = "dasterin" LIMIT 1), (SELECT `id` FROM `Rooms` LIMIT 1));
INSERT INTO `Characters` VALUES (NULL, "Jadael", 10, 10, 10, 10, 10, 10, (SELECT `id` FROM `Users` WHERE `username` = "t" LIMIT 1), (SELECT `id` FROM `Rooms` LIMIT 1));