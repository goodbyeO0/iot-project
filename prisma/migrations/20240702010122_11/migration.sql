/*
  Warnings:

  - You are about to drop the `Record` table. If the table is not empty, all the data it contains will be lost.

*/
-- DropTable
DROP TABLE "Record";

-- CreateTable
CREATE TABLE "entryRecord" (
    "id" SERIAL NOT NULL,
    "UID" TEXT NOT NULL,
    "name" TEXT NOT NULL,
    "dateTime" TIMESTAMP(3) NOT NULL,

    CONSTRAINT "entryRecord_pkey" PRIMARY KEY ("id")
);
