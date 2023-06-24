const API_SERVER = "http://localhost:8080/v0";

export const Config = {
    API_SERVER: API_SERVER,
    POLL_PERIOD_MS: 1000,
    SLOW_POLL_PERIOD_MS: 5000,
    Endpoints: {
        Uptime: `${API_SERVER}/uptime`,
        Mem: `${API_SERVER}/mem`,
        Cpus: `${API_SERVER}/cpus`,
        Procs: `${API_SERVER}/procs`
    }
};