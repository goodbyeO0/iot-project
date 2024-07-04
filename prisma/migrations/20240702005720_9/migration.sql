/*
  Warnings:

  - The primary key for the `Record` table will be changed. If it partially fails, the table could be left without primary key constraint.

*/
-- AlterTable
ALTER TABLE "Record" DROP CONSTRAINT "Record_pkey",
ADD COLUMN     "id" SERIAL NOT NULL,
ADD CONSTRAINT "Record_pkey" PRIMARY KEY ("id");
