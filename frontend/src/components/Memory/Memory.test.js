import React from "react";
import { render, screen, act } from "@testing-library/react";
import Memory from "./Memory";
import { ConnectionStatus } from "../../context/conn-status";
import axios from "axios";
import { Config } from "../../config/config";

jest.useFakeTimers();
jest.mock("axios");

// Advances fake timers by POLL_PERIOD_MS and
// runs any pending promise jobs
const advanceTimeByPollPeriod = async () => {
   await act(async () => {
      jest.advanceTimersByTime(Config.POLL_PERIOD_MS);
      await Promise.resolve();
   });
};

beforeEach(() => {
   // Mock axios to return memory snapshots with 1GB total
   // memory, 0.75GB free and 25% used
   axios.get.mockResolvedValue({
      data: {
         total_memory_kB: 1 * (1024 * 1024),
         free_memory_kB: 0.75 * (1024 * 1024),
         usage_percent: 25,
      },
   });
});

describe("Memory component", () => {
   test("should not fetch mem snapshot after POLL_PERIOD_MS when conn status is not ok", async () => {
      render(<Memory connStatus={ConnectionStatus.Connecting} />);
      await advanceTimeByPollPeriod();

      expect(axios.get).toHaveBeenCalledTimes(0);
   });

   test("should fetch mem snapshot after POLL_PERIOD_MS when conn status is ok", async () => {
      render(<Memory connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      expect(axios.get).toHaveBeenCalledTimes(1);
      expect(axios.get).toHaveBeenCalledWith(Config.Endpoints.Mem);
   });

   test("should display total memory in GBs", async () => {
      render(<Memory connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      await screen.findByText("1.00 GB");
   });

   test("should display total memory in MBs", async () => {
      render(<Memory connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      await screen.findByText("1024 MB");
   });

   test("should display free memory in GBs", async () => {
      render(<Memory connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      await screen.findByText("0.75 GB");
   });

   test("should display free memory in MBs", async () => {
      render(<Memory connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      await screen.findByText("768 MB");
   });

   test("should display usage percentage", async () => {
      render(<Memory connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      await screen.findByText("25%");
   });

   test("should update displayed data when memory snapshot changes", async () => {
      render(<Memory connStatus={ConnectionStatus.Ok} />);

      // Trigger first poll
      await advanceTimeByPollPeriod();
      // Alter mocked memory snapshot result and trigger second poll
      axios.get.mockResolvedValue({
         data: {
            total_memory_kB: 4 * (1024 * 1024),
            free_memory_kB: 1 * (1024 * 1024),
            usage_percent: 75,
         },
      });
      await advanceTimeByPollPeriod();

      await screen.findByText("4.00 GB");
      await screen.findByText("1.00 GB");
      await screen.findByText("75%");
   });
});
