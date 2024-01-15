-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------
-- -----------------------------------------------------
-- Schema IOT
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema IOT
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `IOT` DEFAULT CHARACTER SET utf8mb4 ;
USE `IOT` ;

-- -----------------------------------------------------
-- Table `IOT`.`Dice`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `IOT`.`Dice` (
  `ID` INT(11) NOT NULL AUTO_INCREMENT,
  `TimeStamp` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP(),
  `RolledNumber` INT(11) NULL DEFAULT NULL,
  `ChipID` VARCHAR(45) NULL DEFAULT NULL,
  PRIMARY KEY (`ID`)) 
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4;


-- -----------------------------------------------------
-- Table `IOT`.`Measurement`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `IOT`.`Measurement` (
  `ID` INT(11) NOT NULL AUTO_INCREMENT,
  `TimeStamp` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP(),
  `Measurement` VARCHAR(45) NULL DEFAULT NULL,
  PRIMARY KEY (`ID`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4;


-- -----------------------------------------------------
-- Table `IOT`.`Dice_has_Measurement`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `IOT`.`Dice_has_Measurement` (
  `Dice_ID` INT(11) NOT NULL,
  `Measurement_ID` INT(11) NOT NULL,
  PRIMARY KEY (`Dice_ID`, `Measurement_ID`),
  INDEX `fk_Dice_has_Measurement_Measurement1_idx` (`Measurement_ID` ASC) VISIBLE,
  INDEX `fk_Dice_has_Measurement_Dice_idx` (`Dice_ID` ASC) VISIBLE,
  CONSTRAINT `fk_Dice_has_Measurement_Dice`
    FOREIGN KEY (`Dice_ID`)
    REFERENCES `IOT`.`Dice` (`ID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Dice_has_Measurement_Measurement1`
    FOREIGN KEY (`Measurement_ID`)
    REFERENCES `IOT`.`Measurement` (`ID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
