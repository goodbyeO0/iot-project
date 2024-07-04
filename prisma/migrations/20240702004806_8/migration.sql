/*
  Warnings:

  - You are about to drop the column `time` on the `Record` table. All the data in the column will be lost.
  - Added the required column `dateTime` to the `Record` table without a default value. This is not possible if the table is not empty.

*/
-- AlterTable
ALTER TABLE "Record" DROP COLUMN "time",
ADD COLUMN     "dateTime" TIMESTAMP(3) NOT NULL;
