import React from "react";
import { render, screen, act, waitFor } from "@testing-library/react";
import Overview from "./Overview";
import { ConnectionStatus } from "../../context/conn-status";
import axios from "axios";
import { Config } from "../../config/config";

jest.useFakeTimers();
jest.mock("axios");
const mockSetConnStatus = jest.fn();

// Advances fake timers by poll_period_ms and
// runs any pending promise jobs
const advanceTimeBy = async (poll_period_ms) => {
   await act(async () => {
      jest.advanceTimersByTime(poll_period_ms);
      await Promise.resolve();
   });
};

beforeEach(() => {
   // Mock axios to return uptime
   axios.get.mockResolvedValue({
      data: {
         formatted: "48:06:38",
         hours: 48,
         minutes: 6,
         seconds: 38,
         total_seconds: 173198,
      },
   });
});

describe("Overview component", () => {
   beforeEach(() => {
      jest.resetModules();
   });

   test("should fetch Uptime after first render", async () => {
      render(
         <Overview
            setConnStatus={mockSetConnStatus}
            connStatus={ConnectionStatus.Connecting}
         />
      );
      await waitFor(() => {
         expect(axios.get).toHaveBeenCalledTimes(1);
         expect(axios.get).toHaveBeenCalledWith(Config.Endpoints.Uptime);
      });
   });

   test("should fetch Uptime at reduced poll rate when ConnectionStatus is not `Ok`", async () => {
      render(
         <Overview
            setConnStatus={mockSetConnStatus}
            connStatus={ConnectionStatus.Connecting}
         />
      );
      await advanceTimeBy(Config.SLOW_POLL_PERIOD_MS);

      expect(axios.get).toHaveBeenCalledTimes(2);
      expect(axios.get).toHaveBeenCalledWith(Config.Endpoints.Uptime);
   });

   test("should fetch Uptime at regular poll rate when ConnectionStatus is `Ok`", async () => {
      render(
         <Overview
            setConnStatus={mockSetConnStatus}
            connStatus={ConnectionStatus.Ok}
         />
      );
      await advanceTimeBy(Config.POLL_PERIOD_MS);

      expect(axios.get).toHaveBeenCalledTimes(2);
      expect(axios.get).toHaveBeenCalledWith(Config.Endpoints.Uptime);
   });

   test("should display Uptime in format {hours}:{minutes}:{seconds}, e.g. 48:06:38", async () => {
      render(
         <Overview
            setConnStatus={mockSetConnStatus}
            connStatus={ConnectionStatus.Ok}
         />
      );

      await waitFor(() => {
         expect(screen.getByText("48:06:38")).toBeVisible();
      });
   });

   test("should display Uptime in format {days}d{hours}h{minutes}m, e.g. 2d0h6m", async () => {
      render(
         <Overview
            setConnStatus={mockSetConnStatus}
            connStatus={ConnectionStatus.Ok}
         />
      );

      await waitFor(() => {
         expect(screen.getByText("2d0h6m")).toBeVisible();
      });
   });

   test("should set ConnectionStatus to `Ok` when Uptime is successfully retrieved", async () => {
      render(
         <Overview
            setConnStatus={mockSetConnStatus}
            connStatus={ConnectionStatus.ServerOffline}
         />
      );

      await waitFor(() => {
         expect(mockSetConnStatus).toHaveBeenCalledWith(ConnectionStatus.Ok);
      });
   });

   test("should set ConnectionStatus to `ServerOffline` when Uptime is not successfully retrieved", async () => {
      axios.get.mockRejectedValue("failed to contact server");
      render(
         <Overview
            setConnStatus={mockSetConnStatus}
            connStatus={ConnectionStatus.Ok}
         />
      );

      await waitFor(() => {
         expect(mockSetConnStatus).toHaveBeenCalledWith(
            ConnectionStatus.ServerOffline
         );
      });
   });
});
