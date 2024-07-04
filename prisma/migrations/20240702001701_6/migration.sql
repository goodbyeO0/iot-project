/*
  Warnings:

  - You are about to drop the `Entry` table. If the table is not empty, all the data it contains will be lost.
  - Added the required column `date` to the `Record` table without a default value. This is not possible if the table is not empty.
  - Added the required column `time` to the `Record` table without a default value. This is not possible if the table is not empty.

*/
-- AlterTable
ALTER TABLE "Record" ADD COLUMN     "date" TIMESTAMP(3) NOT NULL,
ADD COLUMN     "time" TIMESTAMP(3) NOT NULL;

-- DropTable
DROP TABLE "Entry";

-- CreateTable
CREATE TABLE "User" (
    "UID" TEXT NOT NULL,
    "name" TEXT NOT NULL,

    CONSTRAINT "User_pkey" PRIMARY KEY ("UID")
);
