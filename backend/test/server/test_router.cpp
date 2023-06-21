#include <gtest/gtest.h>

#include <api_server/logger.h>
#include <api_server/server/router.h>
#include <api_server/server/types.h>
#include <api_server/server/responses.h>

using namespace server;

class RouterTest : public ::testing::Test {
protected:
    void SetUp() override
    {}

    void AddMockEndpoint(const bb::http::verb method, const std::string& target, const HttpResponse& response)
    {
        router.add_route(method, target, [this, response](const auto& req){
            req_recvd_by_endpoint = req;
            return response;
        });
    }

    StdStreamLogger logger{LogLevel::Debug};
    Router router{logger};
    std::optional<HttpRequest> req_recvd_by_endpoint;
};
 
// GIVEN router has no routes
// WHEN router gets request for resource
// THEN router returns bad request response
TEST_F(RouterTest, NoRoutes) {
    HttpRequest request{bb::http::verb::get, "/resource", 1};
    HttpResponse response = router.process_http_request(request);
    ASSERT_EQ(response.result(), bb::http::status::bad_request);
    ASSERT_EQ(response.version(), request.version());
}

// GIVEN router has no routes
// WHEN router gets request with empty URI
// THEN router returns bad request response
TEST_F(RouterTest, EmptyURI) {
    HttpRequest request{bb::http::verb::get, "", 1};
    HttpResponse response = router.process_http_request(request);
    ASSERT_EQ(response.result(), bb::http::status::bad_request);
    ASSERT_EQ(response.version(), request.version());
}

// GIVEN router has multiple GET endpoints
// WHEN router gets request for endpoint resource
// THEN endpoint is called
// AND router returns endpoint's response
TEST_F(RouterTest, GetResourceOk) {
    HttpResponse expected = responses::Ok(1, true, "body");
    AddMockEndpoint(bb::http::verb::get, "/resource", expected);
    AddMockEndpoint(bb::http::verb::get, "/resource2", responses::NotFound(1, true, ""));

    HttpRequest request{bb::http::verb::get, "/resource", 1};
    HttpResponse response = router.process_http_request(request);

    ASSERT_EQ(response.result(), expected.result());
    ASSERT_EQ(response.keep_alive(), expected.keep_alive());
    ASSERT_EQ(response.body(), expected.body());
}

// GIVEN router has a GET endpoint
// WHEN router gets request for endpoint resource with multi-part path
// THEN endpoint is called
TEST_F(RouterTest, GetResourceLongerPathOk) {
    AddMockEndpoint(bb::http::verb::get, "/longer/path", responses::Ok(1, true, "body"));

    HttpRequest request{bb::http::verb::get, "/longer/path", 1};
    (void)router.process_http_request(request);

    ASSERT_TRUE(req_recvd_by_endpoint);
}

// GIVEN router has multiple POST endpoints
// WHEN router gets request for endpoint resource
// THEN endpoint is called
// AND router returns endpoint's response
TEST_F(RouterTest, PostResourceOk) {
    HttpResponse expected = responses::Ok(1, true, "body");
    AddMockEndpoint(bb::http::verb::post, "/resource", expected);
    AddMockEndpoint(bb::http::verb::post, "/resource2", responses::NotFound(1, true, ""));

    HttpRequest request{bb::http::verb::post, "/resource", 1};
    HttpResponse response = router.process_http_request(request);

    ASSERT_EQ(response.result(), expected.result());
    ASSERT_EQ(response.keep_alive(), expected.keep_alive());
    ASSERT_EQ(response.body(), expected.body());
}

// GIVEN router has GET endpoint 
// WHEN router receives request for URI that contains a #fragment
// THEN endpoint receives request with fragment field set
// AND the request's resource path excludes the fragment substring
TEST_F(RouterTest, ParseUriFragment) {
    AddMockEndpoint(bb::http::verb::get, "/resource", responses::Ok(1, true, "body"));

    HttpRequest request{bb::http::verb::get, "/resource#fragment", 1};
    (void)router.process_http_request(request);

    ASSERT_TRUE(req_recvd_by_endpoint.has_value());
    ASSERT_EQ(req_recvd_by_endpoint->fragment(), "fragment");
    ASSERT_EQ(req_recvd_by_endpoint->resource_path(), "/resource");
}

// GIVEN router has GET endpoint 
// WHEN router receives request for URI that contains query params
// THEN endpoint receives request with query parameter map
// AND the request's resource path excludes the query substring
TEST_F(RouterTest, ParseUriQuery) {
    AddMockEndpoint(bb::http::verb::get, "/resource", responses::Ok(1, true, "body"));

    HttpRequest request{bb::http::verb::get, "/resource?key1=value1&key2=value2", 1};
    (void)router.process_http_request(request);

    ASSERT_TRUE(req_recvd_by_endpoint.has_value());
    ASSERT_EQ(req_recvd_by_endpoint->query_parameters().size(), 2);
    ASSERT_EQ(req_recvd_by_endpoint->query_parameters().at("key1"), "value1");
    ASSERT_EQ(req_recvd_by_endpoint->query_parameters().at("key2"), "value2");
    ASSERT_EQ(req_recvd_by_endpoint->resource_path(), "/resource");
}

