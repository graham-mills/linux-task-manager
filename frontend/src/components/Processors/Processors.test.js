import React from "react";
import { render, screen, act } from "@testing-library/react";
import Processors from "./Processors";
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

const CPU_SNAPSHOTS = [
   { id: "cpu", usage_percent: 1.11 },
   { id: "cpu0", usage_percent: 2.22 },
];

beforeEach(() => {
   // Mock axios to return CPU snapshots
   axios.get.mockResolvedValue({
      data: CPU_SNAPSHOTS,
   });
});

describe("Processors component", () => {
   test("should not fetch CPU snapshots after POLL_PERIOD_MS when conn status is not ok", async () => {
      render(<Processors connStatus={ConnectionStatus.Connecting} />);
      await advanceTimeByPollPeriod();

      expect(axios.get).toHaveBeenCalledTimes(0);
   });

   test("should fetch CPU snapshots after POLL_PERIOD_MS when conn status is ok", async () => {
      render(<Processors connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      expect(axios.get).toHaveBeenCalledTimes(1);
      expect(axios.get).toHaveBeenCalledWith(Config.Endpoints.Cpus);
   });

   test("should display ID of each CPU snapshot", async () => {
      render(<Processors connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      for (const snapshot of CPU_SNAPSHOTS) {
         await screen.findByText(snapshot.id);
      }
   });

   test("should display usage percentage of each CPU snapshot", async () => {
      render(<Processors connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      for (const snapshot of CPU_SNAPSHOTS) {
         await screen.findByText(`${snapshot.usage_percent.toFixed(0)}%`);
      }
   });

   test("should display aggregate usage percentage for all CPUs", async () => {
      render(<Processors connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      expect(await screen.findByText("1.11%")).toBeVisible();
   });
});
